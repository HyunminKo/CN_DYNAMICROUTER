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
			return i;		//일치하는 IP address가 있는 포인터 번호 return
		}			
	}

	return -1;			//일치하는 경우가 없을 경우 -1 return
}

int CArpLayer::FindAvailableCacheNum()
{
	for(int i=0;i<MAX_ARP_CACHE;i++){
		if(!arpCache[i])
			return i;		//첫번째 NULL포인터의 위치
	}	
	return -1;			//비어있는 포인터가 없으면 -1 return
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
	
	memcpy(temp.queue_ip,m_header.arp_targetIPAddr,4);				//큐에 넣기위한 변수를 만든다
	memcpy(temp.queue_data,m_header.arp_data,nlength);
	temp.queue_dataSize=nlength;	

	compatibleCacheNum=FindCompatibleCache(dstIPAddr);				//일치하는 cache를 찾는다	

	if(compatibleCacheNum == -1){									//일치하는 cache가 없을경우		
		
		m_header.arp_op=TYPE_ARP_REQUEST;							//request요청		
		availableCacheNum=FindAvailableCacheNum();		
		arpCache[availableCacheNum]=(ptrARP_CACHE)malloc(sizeof(ARP_CACHE));
		memcpy(arpCache[availableCacheNum]->arp_ip,dstIPAddr,4);
		for(int i=0;i<6;i++)
			arpCache[availableCacheNum]->arp_mac[i]=0x00;			//mac Address를 0으로 초기화
		arpCache[availableCacheNum]->arp_state=FALSE;
		arpCache[availableCacheNum]->arp_cache_ttl=10;			//incomplete상태일때 삭제를 위한 부분
		sprintf(arpCache[availableCacheNum]->arp_NIC,"%s",m_NICDescription);
		
		if( !is_full() && (temp.queue_dataSize != 0) )				//데이터크기가 0이면 큐에 넣지 않는다.
			addQueue(temp);											//데이터를 큐에 추가

		if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,broadCasting,TRUE))
			bSuccess=TRUE;	
	}
	else{															//일치하는 cache가 있을 경우
		if(arpCache[compatibleCacheNum]->arp_state==TRUE){		//유효한 cache의 경우
			
			if(nlength==0){											//gratuitous ARP를 위한 ARP Request 패킷
				m_header.arp_op=TYPE_ARP_REQUEST;					//IP부분을 제외한 ARP 패킷만을 보낸다.
				if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,broadCasting,TRUE))
					bSuccess=TRUE;
			}
			else{													//유효한 cache일 경우 ARP패킷을 빼고 IP패킷을 보낸다.
				if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header.arp_data,nlength,
				arpCache[compatibleCacheNum]->arp_mac,FALSE))
					bSuccess=TRUE;
			}			
		}
		else{														//cache는 있지만 유효하지 않을 경우
			m_header.arp_op=TYPE_ARP_REQUEST;						//다시 request를 보낸다
			if( !is_full() && (temp.queue_dataSize != 0) )			//데이터크기가 0이면 큐에 넣지 않는다.
				addQueue(temp);										//데이터를 큐에 추가
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
			arpCache[compatibleCacheNum]->arp_state=TRUE;			//reply를 받은 경우 CacheTable을 유효한 값으로 채운다.
			arpCache[compatibleCacheNum]->arp_cache_ttl=20;	
			CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)m_appWnd;
			pDlg->RefreshCacheTable();
			QUEUE temp;												//큐에서 필요없는 데이터는 삭제
			while( !is_empty() && memcmp((temp=deleteQueue()).queue_ip,arpCache[compatibleCacheNum]->arp_ip,4) );
																	//주소가 일치하는 데이터를 reply가 온 주소로 전송.	
			if( !memcmp(temp.queue_ip,arpCache[compatibleCacheNum]->arp_ip,4) ){
				Send(temp.queue_data,temp.queue_dataSize,temp.queue_ip,FALSE);	
				bSuccess=TRUE;
			}
		}

	}
	else if(header->arp_op==TYPE_ARP_REQUEST ){
			
		if( !memcmp(header->arp_targetIPAddr,m_header.arp_senderIPAddr,4)	//내 ip주소가 맞는지 검사
			|| (bProxyValue=FindCompatibleProxy(header->arp_targetIPAddr)) ){  //proxy에 있는 주소인지 검사
				
		
			if( (compatibleCacheNum=FindCompatibleCache(header->arp_senderIPAddr)) == -1 ){
																			//request를 받았을 경우에도 자신에게
				availableCacheNum=FindAvailableCacheNum();					//해당하는 주소이면 Cache Table을 만든다.		
				arpCache[availableCacheNum]=(ptrARP_CACHE)malloc(sizeof(ARP_CACHE));				
				memcpy(arpCache[availableCacheNum]->arp_ip,header->arp_senderIPAddr,4);
				memcpy(arpCache[availableCacheNum]->arp_mac,header->arp_senderEthernetAddr,6);
				arpCache[availableCacheNum]->arp_state=TRUE;	
				arpCache[availableCacheNum]->arp_cache_ttl=20;				
				sprintf(arpCache[availableCacheNum]->arp_NIC,"%s",m_NICDescription);
				CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)m_appWnd;
				pDlg->RefreshCacheTable();				
			}			
			else{															//Gratuitous request를 받았을경우
																			//Mac주소를 갱신한다.
				if( memcmp(arpCache[compatibleCacheNum]->arp_mac,header->arp_senderEthernetAddr,6) ){
					memcpy(arpCache[compatibleCacheNum]->arp_mac,header->arp_senderEthernetAddr,6);
					CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)m_appWnd;
					pDlg->RefreshCacheTable();
				}
			}			
																			//request에 응답한다.
			memcpy(m_header.arp_targetEthernetAddr,header->arp_senderEthernetAddr,6);
			memcpy(m_header.arp_targetIPAddr,header->arp_senderIPAddr,4);
			m_header.arp_hardType=TYPE_ARP_HARD_TYPE;
			m_header.arp_protType=TYPE_ARP_PROT_TYPE;
			m_header.arp_hardSize=6;
			m_header.arp_protSize=4;
			m_header.arp_op=TYPE_ARP_REPLY;							//상대방이 reply받을 수 있도록
			if(bProxyValue){										//reply 헤더의senderIP부분을 ProxyTable에 있는
				memcpy(tempSenderIP,m_header.arp_senderIPAddr,4);	//값으로 바꾸어서 전송한다.
				memcpy(m_header.arp_senderIPAddr,header->arp_targetIPAddr,4);
			}
			
			if(((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_ARP_HEADER,
				header->arp_senderEthernetAddr,TRUE))
				bSuccess=TRUE;			
			if(bProxyValue)											//헤더의 senderIP부분을 원래대로 복구
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
	for(int i=0;i<MAX_ARP_PROXY;i++){			//ProxyTable을 검색해서 장치명과 IP주소가 일치하는 요소를 찾는다.
		if(arpProxy[i])							//찾으면 TRUE 리턴, 없을 경우 FALSE 리턴	
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
