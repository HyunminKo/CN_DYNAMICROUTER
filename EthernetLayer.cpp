// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EthernetLayer.h"
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

CEthernetLayer::CEthernetLayer(char* pName)
: CBaseLayer( pName )
{

}

CEthernetLayer::~CEthernetLayer()
{

}

BOOL CEthernetLayer::Send( unsigned char* ppayload, int nlength,unsigned char *dstMACaddr,BOOL bArpType)
{
	BOOL bSuccess=FALSE;
	
	memcpy(m_header.enet_data,ppayload,nlength);
	memcpy(m_header.enet_desAddr.S_un.s_ether_addr,dstMACaddr,6);

	if(bArpType)
		m_header.enet_frameType=TYPE_ETHERNET_ARPTYPE;
	else
		m_header.enet_frameType=TYPE_ETHERNET_IPTYPE;

	if(mp_UnderLayer->Send((unsigned char*)&m_header,nlength+SIZE_ETHERNET_HEADER))
		bSuccess=TRUE;

	return bSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char *ppayload)
{
	
	BOOL bSuccess=FALSE;
	ptrETHERNET header=(ptrETHERNET)ppayload;	
	unsigned char broadCasting[6];
	unsigned char multiCasting[6];


	for(int i=0;i<6;i++){
		broadCasting[i]=0xff;
	}

	multiCasting[0]=0x01;
	multiCasting[1]=0x00;
	multiCasting[2]=0x5e;
	multiCasting[3]=0x00;
	multiCasting[4]=0x00;
	multiCasting[5]=0x09;
																				//받은 패킷의목적지주소가 내주소일때
	if( (!memcmp(header->enet_desAddr.S_un.s_ether_addr,m_header.enet_srcAddr.S_un.s_ether_addr,6) 
		|| !memcmp(header->enet_desAddr.S_un.s_ether_addr,broadCasting,6)
		|| !memcmp(header->enet_desAddr.S_un.s_ether_addr,multiCasting,6))  //받은 패킷의 목적지주소가 broadcasting일때		
		&& memcmp(header->enet_srcAddr.S_un.s_ether_addr,m_header.enet_srcAddr.S_un.s_ether_addr,6)){		
																			//받은 패킷의 보내는주소가 내주소와 같지 않을때
			
			if(header->enet_frameType==TYPE_ETHERNET_ARPTYPE){			
				if(mp_aUpperLayer[0]->Receive((unsigned char*)header->enet_data))
				bSuccess=TRUE;				
			}
			else if(header->enet_frameType==TYPE_ETHERNET_IPTYPE){												
				if(mp_aUpperLayer[1]->Receive((unsigned char*)header->enet_data))					
					bSuccess=TRUE;
			}							
	}
	
	return bSuccess;
}


void CEthernetLayer::SetSrcMacAddr(unsigned char *srcAddr )
{
	memcpy(m_header.enet_srcAddr.S_un.s_ether_addr,srcAddr,6);
}