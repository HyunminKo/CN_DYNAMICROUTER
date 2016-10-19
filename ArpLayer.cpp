// ArpLayer.cpp: implementation of the CArpLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouterDlg.h"
#include "ArpLayer.h"
//#include "afxmt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//CCriticalSection g_cs;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArpLayer::CArpLayer(char *pName)
	: CBaseLayer( pName )
{
	rear=0;
	front=0;
}

CArpLayer::~CArpLayer()
{
	
}

int CArpLayer::FindCompatibleCache(unsigned char *dstIPAddr)
{
	
	for(int i=0;i<MAX_ARP_CACHE;i++){			
		if(arpCache[i] && (!memcmp(arpCache[i]->arp_ip,dstIPAddr,4)) ){
			return i;		//��ġ�ϴ� IP address�� �ִ� ������ ��ȣ return
		}			
	}

	return -1;			//��ġ�ϴ� ��찡 ���� ��� -1 return
}

int CArpLayer::FindAvailableCacheNum()
{
	for(int i=0;i<MAX_ARP_CACHE;i++){
		if(!arpCache[i])
			return i;		//ù��° NULL�������� ��ġ
	}	
	return -1;			//����ִ� �����Ͱ� ������ -1 return
}

BOOL CArpLayer::Send(unsigned char* ppayload, int nlength, unsigned char *dstIPAddr,BOOL bRIP)
{	
	
	BOOL bSuccess=FALSE;	
	QUEUE temp;
	unsigned char broadCasting[6];	
	int compatibleCacheNum=0,availableCacheNum=0;

	for(int i=0;i<6;i++)
		broadCasting[i]=0xff;

	if(bRIP){
		if(((CEthernetLayer*)mp_UnderLayer)->Send(ppayload,nlength,broadCasting,FALSE))
			bSuccess=TRUE;
		return bSuccess;
	}	
	m_header.arp_hardType=TYPE_ARP_HARD_TYPE;
	m_header.arp_protType=TYPE_ARP_PROT_TYPE;
	m_header.arp_hardSize=6;
	m_header.arp_protSize=4;
	m_header.arp_op=TYPE_ARP_NOTHING;

	memcpy(m_header.arp_targetIPAddr,dstIPAddr,4);
	memcpy(m_header.arp_data,ppayload,nlength);	
	
	memcpy(temp.queue_ip,m_header.arp_targetIPAddr,4);				//ť�� �ֱ����� ������ �����
	memcpy(temp.queue_data,m_header.arp_data,nlength);
	temp.queue_dataSize=nlength;	

	compatibleCacheNum=FindCompatibleCache(dstIPAddr);				//��ġ�ϴ� cache�� ã�´�	

	if(compatibleCacheNum == -1){									//��ġ�ϴ� cache�� �������		
		
		m_header.arp_op=TYPE_ARP_REQUEST;							//request��û		
		availableCacheNum=FindAvailableCacheNum();		
		arpCache[availableCacheNum]=(ptrARP_CACHE)malloc(sizeof(ARP_CACHE));
		memcpy(arpCache[availableCacheNum]->arp_ip,dstIPAddr,4);
		for(int i=0;i<6;i++)
			arpCache[availableCacheNum]->arp_mac[i]=0x00;			//mac Address�� 0���� �ʱ�ȭ
		arpCache[availableCacheNum]->arp_state=FALSE;
		arpCache[availableCacheNum]->arp_cache_ttl=10;			//incomplete�����϶� ������ ���� �κ�
		sprintf(arpCache[availableCacheNum]->arp_NIC,"%s",m_NICDescription);
		
		if( !is_full() && (temp.queue_dataSize != 0) )				//������ũ�Ⱑ 0�̸� ť�� ���� �ʴ´�.
			addQueue(temp);											//�����͸� ť�� �߰�

		if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,broadCasting,TRUE))
			bSuccess=TRUE;	
	}
	else{															//��ġ�ϴ� cache�� ���� ���
		if(arpCache[compatibleCacheNum]->arp_state==TRUE){		//��ȿ�� cache�� ���
			
			if(nlength==0){											//gratuitous ARP�� ���� ARP Request ��Ŷ
				m_header.arp_op=TYPE_ARP_REQUEST;					//IP�κ��� ������ ARP ��Ŷ���� ������.
				if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,broadCasting,TRUE))
					bSuccess=TRUE;
			}
			else{													//��ȿ�� cache�� ��� ARP��Ŷ�� ���� IP��Ŷ�� ������.
				if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header.arp_data,nlength,
				arpCache[compatibleCacheNum]->arp_mac,FALSE))
					bSuccess=TRUE;
			}			
		}
		else{														//cache�� ������ ��ȿ���� ���� ���
			m_header.arp_op=TYPE_ARP_REQUEST;						//�ٽ� request�� ������
			if( !is_full() && (temp.queue_dataSize != 0) )			//������ũ�Ⱑ 0�̸� ť�� ���� �ʴ´�.
				addQueue(temp);										//�����͸� ť�� �߰�
			if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,broadCasting,TRUE))
			bSuccess=TRUE;			
		}
	}		
	
	return bSuccess;
}

BOOL CArpLayer::Receive(unsigned char *ppayload)
{
	
	BOOL bSuccess=FALSE, bProxyValue=FALSE;
	ptrARP header=(ptrARP)ppayload;	
	int compatibleCacheNum=0,availableCacheNum=0;
	unsigned char tempSenderIP[4];
	
	if( header->arp_op==TYPE_ARP_REPLY && !memcmp(header->arp_targetIPAddr,m_header.arp_senderIPAddr,4) ){
		
		if( (compatibleCacheNum=FindCompatibleCache(header->arp_senderIPAddr)) != -1 ){
			memcpy(arpCache[compatibleCacheNum]->arp_mac,header->arp_senderEthernetAddr,6);
			arpCache[compatibleCacheNum]->arp_state=TRUE;			//reply�� ���� ��� CacheTable�� ��ȿ�� ������ ä���.
			arpCache[compatibleCacheNum]->arp_cache_ttl=20;	
			CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)m_appWnd;
			pDlg->RefreshCacheTable();
			QUEUE temp;												//ť���� �ʿ���� �����ʹ� ����
			while( !is_empty() && memcmp((temp=deleteQueue()).queue_ip,arpCache[compatibleCacheNum]->arp_ip,4) );
																	//�ּҰ� ��ġ�ϴ� �����͸� reply�� �� �ּҷ� ����.	
			if( !memcmp(temp.queue_ip,arpCache[compatibleCacheNum]->arp_ip,4) ){
				Send(temp.queue_data,temp.queue_dataSize,temp.queue_ip,FALSE);	
				bSuccess=TRUE;
			}
		}

	}
	else if(header->arp_op==TYPE_ARP_REQUEST ){
			
		if( !memcmp(header->arp_targetIPAddr,m_header.arp_senderIPAddr,4)	//�� ip�ּҰ� �´��� �˻�
			|| (bProxyValue=FindCompatibleProxy(header->arp_targetIPAddr)) ){  //proxy�� �ִ� �ּ����� �˻�
				
		
			if( (compatibleCacheNum=FindCompatibleCache(header->arp_senderIPAddr)) == -1 ){
																			//request�� �޾��� ��쿡�� �ڽſ���
				availableCacheNum=FindAvailableCacheNum();					//�ش��ϴ� �ּ��̸� Cache Table�� �����.		
				arpCache[availableCacheNum]=(ptrARP_CACHE)malloc(sizeof(ARP_CACHE));				
				memcpy(arpCache[availableCacheNum]->arp_ip,header->arp_senderIPAddr,4);
				memcpy(arpCache[availableCacheNum]->arp_mac,header->arp_senderEthernetAddr,6);
				arpCache[availableCacheNum]->arp_state=TRUE;	
				arpCache[availableCacheNum]->arp_cache_ttl=20;				
				sprintf(arpCache[availableCacheNum]->arp_NIC,"%s",m_NICDescription);
				CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)m_appWnd;
				pDlg->RefreshCacheTable();				
			}			
			else{															//Gratuitous request�� �޾������
																			//Mac�ּҸ� �����Ѵ�.
				if( memcmp(arpCache[compatibleCacheNum]->arp_mac,header->arp_senderEthernetAddr,6) ){
					memcpy(arpCache[compatibleCacheNum]->arp_mac,header->arp_senderEthernetAddr,6);
					CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)m_appWnd;
					pDlg->RefreshCacheTable();
				}
			}			
																			//request�� �����Ѵ�.
			memcpy(m_header.arp_targetEthernetAddr,header->arp_senderEthernetAddr,6);
			memcpy(m_header.arp_targetIPAddr,header->arp_senderIPAddr,4);
			m_header.arp_hardType=TYPE_ARP_HARD_TYPE;
			m_header.arp_protType=TYPE_ARP_PROT_TYPE;
			m_header.arp_hardSize=6;
			m_header.arp_protSize=4;
			m_header.arp_op=TYPE_ARP_REPLY;							//������ reply���� �� �ֵ���
			if(bProxyValue){										//reply �����senderIP�κ��� ProxyTable�� �ִ�
				memcpy(tempSenderIP,m_header.arp_senderIPAddr,4);	//������ �ٲپ �����Ѵ�.
				memcpy(m_header.arp_senderIPAddr,header->arp_targetIPAddr,4);
			}
			
			if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,
				header->arp_senderEthernetAddr,TRUE))
				bSuccess=TRUE;			
			if(bProxyValue)											//����� senderIP�κ��� ������� ����
				memcpy(m_header.arp_senderIPAddr,tempSenderIP,4);
		}

	}	
	
	return bSuccess;
}



void CArpLayer::addQueue(QUEUE elem)
{
	rear=(rear+1) % MAX_QUEUE;
	memcpy(m_queue[rear].queue_data,elem.queue_data,elem.queue_dataSize);
	memcpy(m_queue[rear].queue_ip,elem.queue_ip,4);
	m_queue[rear].queue_dataSize=elem.queue_dataSize;	
}

CArpLayer::QUEUE CArpLayer::deleteQueue()
{
	front=(front+1) % MAX_QUEUE;
	return m_queue[front];	
}

int CArpLayer::is_empty()
{
	if(rear==front)
		return 1;
	else
		return 0;
}

int CArpLayer::is_full()
{
	if( (rear+1)%5 ==  front)
		return 1;
	else
		return 0;
}

BOOL CArpLayer::FindCompatibleProxy(unsigned char *ipAddr)
{
	for(int i=0;i<MAX_ARP_PROXY;i++){			//ProxyTable�� �˻��ؼ� ��ġ��� IP�ּҰ� ��ġ�ϴ� ��Ҹ� ã�´�.
		if(arpProxy[i])							//ã���� TRUE ����, ���� ��� FALSE ����	
			if( !memcmp(m_NICDescription,arpProxy[i]->MacAddrDescrip,arpProxy[i]->DescriptoinSize) &&
				!memcmp(ipAddr,arpProxy[i]->IpAddress,4))
				return TRUE;		
	}
	return FALSE;
}

void CArpLayer::SetAddress(unsigned char *srcIPAddr,unsigned char *srcMacAddr,char *NICDescription)
{
	memcpy(m_header.arp_senderIPAddr,srcIPAddr,4);
	memcpy(m_header.arp_senderEthernetAddr,srcMacAddr,6);
	sprintf(m_NICDescription,"%s",NICDescription);	
}
