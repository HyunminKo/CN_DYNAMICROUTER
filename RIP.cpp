// RIP.cpp: implementation of the CRIP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouter.h"
#include "RIP.h"
//#include "afxmt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//CCriticalSection g_cs2;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRIP::CRIP(char* pName)
	: CBaseLayer( pName )
{
	
}

CRIP::~CRIP()
{

}

BOOL CRIP::Send(int RIPtype)
{	
	
	BOOL bSuccess=FALSE;
	CUDP* layer=NULL;
	int num=0,i=0,n=0;	
	unsigned char tempIP[4]={0x00};

	switch(RIPtype){

		case 1:
			m_header.rm_cmd=TYPE_RIP_REQUEST;											//RIP Request를 요청한다.
			m_header.rm_version=TYPE_RIP_VERSION;
			m_header.rm_unused=0x0000;
			m_header.rm_addr[0].rm_addrfamily=ntohs(0);
			memset(m_header.rm_addr[0].rm_ip.S_ip_addr,0,4);
			memset(m_header.rm_addr[0].rm_subnetmask.S_ip_addr,0,4);
			memset(m_header.rm_addr[0].rm_nexthopip.S_ip_addr,0,4);			
			m_header.rm_addr[0].rm_metric=ntohl(16);
			
			for(i=0;i<MAX_ADAPTER_INFO;i++){
				if(((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]){
					layer=(CUDP*)((CDynamicRouterDlg*)mp_aUpperLayer[0])->FindCompatibleLayer(1,((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]->MacAddrDescrip);
					if(layer->Send((unsigned char*)&m_header,24))
						bSuccess=TRUE;
				}
			}
			break;

		case 2:
			m_header.rm_cmd=TYPE_RIP_RESPONSE;									//RIP Response를 생성한다.
			m_header.rm_version=TYPE_RIP_VERSION;
			m_header.rm_unused=0x0000;
			
			for(n=0;n<MAX_ROUTING_TABLE;n++){
				if(m_routingTable[n]){			
					m_header.rm_addr[num].rm_addrfamily=TYPE_RIP_ADDRFAMILY;
					m_header.rm_addr[num].rm_routetag=TYPE_RIP_ROUTETAG;		
					memcpy(m_header.rm_addr[num].rm_ip.S_ip_addr,m_routingTable[n]->destination.S_ip_addr,4);
					memcpy(m_header.rm_addr[num].rm_subnetmask.S_ip_addr,m_routingTable[n]->netmask.S_ip_addr,4);					
					m_header.rm_addr[num++].rm_metric=ntohl(m_routingTable[n]->metric);			
				}
			}

			
			for(i=0;i<MAX_ADAPTER_INFO;i++){
				if(((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]){			//각장치마다 전송
					layer=(CUDP*)((CDynamicRouterDlg*)mp_aUpperLayer[0])->FindCompatibleLayer(1,((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]->MacAddrDescrip);
					for(n=0;n<num;n++){
						memcpy(m_header.rm_addr[n].rm_nexthopip.S_ip_addr,tempIP,4);
					}
					if(layer->Send((unsigned char*)&m_header,num*20+4))
						bSuccess=TRUE;
				}
			}

			break;
	}
	
	return bSuccess;
}
BOOL CRIP::Receive(unsigned char *ppayload, char* NICDescription, unsigned char* srcIPAddr)
{

	BOOL bSuccess=FALSE;
	ptrRIP header=(ptrRIP)ppayload;	
	CUDP* layer=NULL;
	int i=0,n=0,num=0,availableNum=0, redundancyNum=0;
	BOOL update[MAX_ROUTING_TABLE];
	unsigned char tempIP[4]={0x00};
	CString str1,str2;
	
	str1.Format("%s",NICDescription);
	for(i=0;i<MAX_ROUTING_TABLE;i++)
		update[i]=FALSE;
								
	// RIP_REQUEST를 받은 경우
	if(header->rm_cmd==TYPE_RIP_REQUEST){
		if( ( ntohs(header->rm_addr[0].rm_addrfamily) == 0 ) && ( ntohs(header->rm_addr[0].rm_metric) == 16 ) ){	
			m_header.rm_cmd=TYPE_RIP_RESPONSE;		//Response 패킷을 생성한다.	 
			m_header.rm_version=TYPE_RIP_VERSION;
			m_header.rm_unused=0x0000;
			
			// 패킷의 내용으로 전체 테이블을 넣는다.							
			for(n=0;n<MAX_ROUTING_TABLE;n++){								
				if(m_routingTable[n]){			
					m_header.rm_addr[num].rm_addrfamily=TYPE_RIP_ADDRFAMILY;
					m_header.rm_addr[num].rm_routetag=TYPE_RIP_ROUTETAG;		
					memcpy(m_header.rm_addr[num].rm_ip.S_ip_addr,m_routingTable[n]->destination.S_ip_addr,4);
					memcpy(m_header.rm_addr[num].rm_subnetmask.S_ip_addr,m_routingTable[n]->netmask.S_ip_addr,4);					
					m_header.rm_addr[num++].rm_metric=ntohl(m_routingTable[n]->metric);			
				}
			}
			
			for(i=0;i<MAX_ADAPTER_INFO;i++){
				if(((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]){			//각 장치마다 전송
					layer=(CUDP*)((CDynamicRouterDlg*)mp_aUpperLayer[0])->FindCompatibleLayer(1,((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]->MacAddrDescrip);				
					for(n=0;n<num;n++){
						memcpy(m_header.rm_addr[n].rm_nexthopip.S_ip_addr,tempIP,4);
					}
					str2.Format("%s",((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]->MacAddrDescrip);			
					if( !str1.Compare(str2) ){
						if( (!layer->Send((unsigned char*)&m_header,4)) ) 
							AfxMessageBox("RIP Request Sending Error");
					}
				}
			}
			num=0;
		}
		
	}
	// RIP_RESPENSE를 받을경우			
	else if(header->rm_cmd==TYPE_RIP_RESPONSE)
	{							
		for(int i=0;i<25;i++)
		{												
			if( (header->rm_addr[i].rm_addrfamily == TYPE_RIP_ADDRFAMILY) && (header->rm_addr[i].rm_routetag == TYPE_RIP_ROUTETAG) ){											
				// 000.000.000.000 으로 온 경우 해당 정보가 온 곳의 ip를 nexthop으로 설정함 (시스코 때메~)
				if( !memcmp(header->rm_addr[i].rm_nexthopip.S_ip_addr,tempIP,4) )				
					memcpy(header->rm_addr[i].rm_nexthopip.S_ip_addr,srcIPAddr,4);

				// 비교하기전에 metric을 1 증가시킴
				if( ntohl(header->rm_addr[i].rm_metric) < 16 )
					header->rm_addr[i].rm_metric+=ntohl(1);
			
				// 테이블에 없는 네트워크 (새로운 내용임, 추가)
				if( (redundancyNum=FindRedundancy(header->rm_addr[i].rm_ip.S_ip_addr)) == -1)
				{		
					if( (availableNum=FindAvailableStaticTableNum()) != -1 ){
						if( ntohl(header->rm_addr[i].rm_metric) < 16){
							update[availableNum]=TRUE;
							m_routingTable[availableNum]=(CRoutingTable::ptrROUTING_TABLE)malloc(sizeof(CRoutingTable::ROUTING_TABLE));			
							memcpy(m_routingTable[availableNum]->destination.S_ip_addr,header->rm_addr[i].rm_ip.S_ip_addr,4);
							memcpy(m_routingTable[availableNum]->netmask.S_ip_addr,header->rm_addr[i].rm_subnetmask.S_ip_addr,4);
							memcpy(m_routingTable[availableNum]->gateway.S_ip_addr,header->rm_addr[i].rm_nexthopip.S_ip_addr,4);				
							m_routingTable[availableNum]->metric=ntohl(header->rm_addr[i].rm_metric);
							m_routingTable[availableNum]->cnt=0;
							sprintf(m_routingTable[availableNum]->device_descripter,"%s",NICDescription);
							
							if(m_routingTable[availableNum]->metric==1)
								sprintf(m_routingTable[availableNum]->flag,"U");
							else if(m_routingTable[availableNum]->metric>1)
								sprintf(m_routingTable[availableNum]->flag,"UG");
						}

						
					}
					else
						AfxMessageBox("라우팅테이블의 공간이 부족합니다.");

				}
				// 이미 테이블이 존재하는 경우
				else{																		
					// nextHop이 같은 경우 업데이트 (같은 경로에서 온 정보)
					if( !memcmp(m_routingTable[redundancyNum]->gateway.S_ip_addr,header->rm_addr[i].rm_nexthopip.S_ip_addr,4) ){												
						memcpy(m_routingTable[redundancyNum]->destination.S_ip_addr,header->rm_addr[i].rm_ip.S_ip_addr,4);
						memcpy(m_routingTable[redundancyNum]->netmask.S_ip_addr,header->rm_addr[i].rm_subnetmask.S_ip_addr,4);
						
						// 수정되었다면 triggered 예약
						if( (m_routingTable[redundancyNum]->metric != ((int)ntohl(header->rm_addr[i].rm_metric))) ){							
								update[redundancyNum]=TRUE;
						}

						// 현재 metric이 16보다 작을 경우 받은 metric을 받아들인다.
						if( m_routingTable[redundancyNum]->metric < 16)
						{									
							if( ntohl(header->rm_addr[i].rm_metric) < 16 )															
								m_routingTable[redundancyNum]->metric=ntohl(header->rm_addr[i].rm_metric);								
							else{												
								m_routingTable[redundancyNum]->metric=16;								
							}							
							m_routingTable[redundancyNum]->cnt=0;	// counter 초기화
						}
						// 현재 metric이 16일 경우
						else if( m_routingTable[redundancyNum]->metric == 16 )
						{
							// 받은 metric이 16미만일 경우 받아 들인다.
							if( ntohl(header->rm_addr[i].rm_metric) < 16 ){									
								m_routingTable[redundancyNum]->metric=ntohl(header->rm_addr[i].rm_metric);
								m_routingTable[redundancyNum]->cnt=0;
							}																				
							// 받은 metric이 16일 경우 받아 들이지않는다.
						}						
						
						sprintf(m_routingTable[redundancyNum]->device_descripter,"%s",NICDescription);
						
						if(m_routingTable[redundancyNum]->metric==1)
							sprintf(m_routingTable[redundancyNum]->flag,"U");
						else if(m_routingTable[redundancyNum]->metric>1)
							sprintf(m_routingTable[redundancyNum]->flag,"UG");					

					}
					// nextHop이 다른경우 (다른 경로에서 온 정보) metric이 적을 때만 업데이트
					else{																	
						if( m_routingTable[redundancyNum]->metric > ((int)ntohl(header->rm_addr[i].rm_metric)) ){
							update[redundancyNum]=TRUE;
							memcpy(m_routingTable[redundancyNum]->destination.S_ip_addr,header->rm_addr[i].rm_ip.S_ip_addr,4);
							memcpy(m_routingTable[redundancyNum]->netmask.S_ip_addr,header->rm_addr[i].rm_subnetmask.S_ip_addr,4);
							memcpy(m_routingTable[redundancyNum]->gateway.S_ip_addr,header->rm_addr[i].rm_nexthopip.S_ip_addr,4);				
							m_routingTable[redundancyNum]->metric=ntohl(header->rm_addr[i].rm_metric);
							sprintf(m_routingTable[redundancyNum]->device_descripter,"%s",NICDescription);
							if(m_routingTable[redundancyNum]->metric==1)
								sprintf(m_routingTable[redundancyNum]->flag,"U");
							else if(m_routingTable[redundancyNum]->metric>1)
								sprintf(m_routingTable[redundancyNum]->flag,"UG");
							m_routingTable[redundancyNum]->cnt=0;

						}
					}
					
				}					
			}
		}
		// triggered update
		TriggeredUpdate(update,NICDescription);						
		((CDynamicRouterDlg*)mp_aUpperLayer[0])->RefreshRoutingTable();
	}	
	return bSuccess;
}

int CRIP::FindAvailableStaticTableNum()
{
	for(int i=0;i<MAX_ROUTING_TABLE;i++){
		if(!m_routingTable[i])
			return i;
	}

	return -1;
}

// 네트워크 ID가 일치하는 테이블목록이 있는지 검색 (-1이면 없는것)
int CRIP::FindRedundancy(unsigned char *ipAddr)
{
	for(int i=0;i<MAX_ROUTING_TABLE;i++){
		if(m_routingTable[i] && !memcmp(m_routingTable[i]->destination.S_ip_addr,ipAddr,4)){
			return i;
		}
	}
	return -1;
}

// Triggered Update with Split horizons2
void CRIP::TriggeredUpdate(BOOL *update, char* NICDescription)
{
	int i=0,n=0, ripCnt=0;
	CUDP* layer=NULL;
	CString str1,str2;
	unsigned char tempIP[4]={0x00};

	str1.Format("%s",NICDescription);
	m_header.rm_cmd=TYPE_RIP_RESPONSE;									//RIP Response를 생성한다.
	m_header.rm_version=TYPE_RIP_VERSION;
	m_header.rm_unused=0x0000;

	for(i=0;i<MAX_ROUTING_TABLE;i++){									//바뀐내용만 메세지에 포함한다.
		if(update[i]){
			m_header.rm_addr[ripCnt].rm_addrfamily=TYPE_RIP_ADDRFAMILY;
			m_header.rm_addr[ripCnt].rm_routetag=TYPE_RIP_ROUTETAG;		
			memcpy(m_header.rm_addr[ripCnt].rm_ip.S_ip_addr,m_routingTable[i]->destination.S_ip_addr,4);
			memcpy(m_header.rm_addr[ripCnt].rm_subnetmask.S_ip_addr,m_routingTable[i]->netmask.S_ip_addr,4);					
			m_header.rm_addr[ripCnt++].rm_metric=ntohl(m_routingTable[i]->metric);	
			update[i]=FALSE;	
		}
	}
	
	if( ripCnt > 0 )
	{
		for(i=0;i<MAX_ADAPTER_INFO;i++)
		{
			//Split horizons 이용 전송
			if(((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i])
			{			
				layer=(CUDP*)((CDynamicRouterDlg*)mp_aUpperLayer[0])->FindCompatibleLayer(1,((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]->MacAddrDescrip);				
				
				for(n=0;n<ripCnt;n++){
					memcpy(m_header.rm_addr[n].rm_nexthopip.S_ip_addr,tempIP,4);
				}

				str2.Format("%s",((CDynamicRouterDlg*)mp_aUpperLayer[0])->m_Addr[i]->MacAddrDescrip);			
				
				// 정보를 전달받은 곳과 다를경우에만 전송한다.
				if( str1.Compare(str2) )
				{					
					if( (!layer->Send((unsigned char*)&m_header,ripCnt*20+4)) ) 
						AfxMessageBox("RIP Triggered Update Sending Error");
				}				
			}
		}
	}	


}
//추가된 내용