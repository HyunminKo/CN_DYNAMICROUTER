// IpLayer.cpp: implementation of the CIpLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IpLayer.h"

#include "DynamicRouterDlg.h"
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

CIpLayer::CIpLayer(char *pName)
	: CBaseLayer( pName )
{		
	m_ipFragment=0;
}

CIpLayer::~CIpLayer()
{

}

BOOL CIpLayer::Send( unsigned char* ppayload, int nlength ,unsigned char *dstAddr,BOOL bRIP)
{
	BOOL bSuccess=FALSE;
		
	m_header.ip_verlen=TYPE_IP_VERLEN;
	m_header.ip_tos=TYPE_IP_TOS;	
	m_header.ip_len=ntohs(SIZE_IP_HEADER+nlength);
	m_header.ip_id=ntohs(m_ipFragment++);
	m_header.ip_fragoff=TYPE_IP_FRAGOFF;
	m_header.ip_ttl=TYPE_IP_TTL;
	m_header.ip_cksum=0x0000;
	memcpy(m_header.ip_destination.S_ip_addr,dstAddr,4);	
	memcpy(m_header.ip_data,ppayload,nlength);		

	if(bRIP){
		m_header.ip_proto=TYPE_IP_UDPTYPE;
	}	
	MakeCheckSum();
																			//Arp�������� ��Ʈ�� ��Ŷ�� ���������� ������ ��Ŷ��
	if(nlength==0){														//���������� ������ ���� ����� ȣ��		
		if(((CArpLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,nlength,dstAddr,bRIP))	
			bSuccess=TRUE;
	}
	else{
		if(((CArpLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,nlength+SIZE_IP_HEADER,dstAddr,bRIP))	
			bSuccess=TRUE;
	}
	return bSuccess;
}

BOOL CIpLayer::Receive(unsigned char *ppayload)
{
	
	BOOL bSuccess=FALSE;
	CString str,temp;
	int compatibleNum=0;
	ptrIP header=(ptrIP)ppayload;
	CBaseLayer *layer=NULL;
	unsigned short nlength=ntohs(header->ip_len);
	unsigned char multiCasting[4];

	multiCasting[0]=0xe0;
	multiCasting[1]=0x00;
	multiCasting[2]=0x00;
	multiCasting[3]=0x09;
	
	if( memcmp(header->ip_source.S_ip_addr,m_header.ip_source.S_ip_addr,4)
		|| !memcmp(header->ip_destination.S_ip_addr,multiCasting,4) ){

		if( (compatibleNum=FindDstOfTable(header->ip_destination.S_ip_addr)) != -1 ){	//��������̺� ���� �������	
							
			CDynamicRouterDlg* pDlg=(CDynamicRouterDlg*)m_appWnd;			//�����͸� �����ؾ��� �������� ������ �˻�
			layer=pDlg->FindCompatibleLayer(3,m_routingTable[compatibleNum]->device_descripter);
			
			if( strchr(m_routingTable[compatibleNum]->flag,'G') ){		//�ٸ� ��Ʈ��ũ�� �������� ���				
				
				if( ((CArpLayer*)layer)->Send((unsigned char*)header,SIZE_IP_HEADER+nlength,m_routingTable[compatibleNum]->gateway.S_ip_addr,FALSE) )
					bSuccess=TRUE;			
			}
			else if( strchr(m_routingTable[compatibleNum]->flag,'U') ){	//���� ��Ʈ��ũ�� �������� ���
				
				if( ((CArpLayer*)layer)->Send((unsigned char*)header,SIZE_IP_HEADER+nlength,header->ip_destination.S_ip_addr,FALSE) )
					bSuccess=TRUE;					
			}
			//if(header->ip_proto == TYPE_IP_ICMP){
			//	ptrICMP newIcmp = (ICMP*)malloc(sizeof(ICMP));
			//	newIcmp->icmp_type = 0x08;
			//	newIcmp->icmp_cksum = ICMP_checksum((u_short*)&newIcmp,SIZE_ICMP_HEADER+SIZE_ICMP_DATA);
			//	newIcmp->icmp_code = 0x00;
			//	memcpy(header->ip_data,newIcmp,SIZE_ICMP_HEADER+SIZE_ICMP_DATA);
			//	if( ((CArpLayer*)layer)->Send((unsigned char*)header,SIZE_IP_HEADER+nlength,m_routingTable[compatibleNum]->gateway.S_ip_addr,FALSE) )
			//		bSuccess=TRUE;
			//}
		}
		else if(header->ip_proto==TYPE_IP_UDPTYPE){
			((CUDP*)mp_aUpperLayer[0])->Receive((unsigned char*)header->ip_data,
				header->ip_destination.S_ip_addr,header->ip_source.S_ip_addr);
		}
	}
	
	return bSuccess;
}	

int CIpLayer::FindDstOfTable(unsigned char *dstIP)
{

	for(int i=0;i<MAX_ROUTING_TABLE;i++){
		if(m_routingTable[i]){
			if( (!memcmp(m_routingTable[i]->destination.S_ip_addr,IPNetmask(dstIP,m_routingTable[i]->netmask.S_ip_addr),4)) )				
				return i;
		}
	}
	return -1;
}

unsigned char* CIpLayer::IPNetmask(unsigned char *dstIP, unsigned char *nestmask)
{
	unsigned char *dstNetwork=(unsigned char*)malloc(sizeof(unsigned char)*4);
	
	for(int i=0;i<4;i++)
		dstNetwork[i]=dstIP[i] & nestmask[i];

	return dstNetwork;
}
void CIpLayer::SetSrcIPAddr(unsigned char *srcIPAddr,char *NICDescription)
{
	memcpy(m_header.ip_source.S_ip_addr,srcIPAddr,4);
	sprintf(m_NICDescription,"%s",NICDescription);
}

void CIpLayer::MakeCheckSum()
{
	
	int iSize = SIZE_IP_HEADER;
	unsigned short* usBuff = new unsigned short[iSize/2];
	unsigned long usChksum= 0 ;

	memcpy(usBuff, &m_header, iSize);

	while(iSize>1)
	{
		usChksum += *usBuff++;
		iSize -= sizeof(unsigned short);
	}

	if(iSize)
		usChksum += *(unsigned char*)usBuff;

	usChksum = (usChksum >> 16) + (usChksum & 0xffff);
	usChksum += (usChksum >> 16);

	m_header.ip_cksum = (unsigned short)(~usChksum);
}
int CIpLayer::ICMP_checksum(u_short* data,int len)
{
	register u_short answer;
	register long sum = 0;
	u_short odd_byte=0;

	while(len>1){
		sum += *data++;
		len -= 2;
	}

	if(len==1){
		*(u_char*)(&odd_byte) = *(u_char*)data;
		sum += odd_byte;
	}

	sum = (sum>>16) + (sum&0xffff); // ���� 16��Ʈ�� ���� 16��Ʈ�� ���Ѵ�.
	sum += (sum>>16); // add carry
	return ~sum; // 1�� ����
}

BOOL CIpLayer::IsCorrectCheckSum(ptrIP header)
{
	//header�� ���ؼ��� üũ���Ѵ����� üũ���� ������ TRUE, �ƴϸ� FALSE
	return FALSE;
}
