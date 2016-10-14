// PacketDriverLayer.cpp: implementation of the CPacketDriverLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PacketDriverLayer.h"
#include "pcap.h"
#include "packet32.h"
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

CPacketDriverLayer::CPacketDriverLayer(char *pName)
: CBaseLayer( pName )
{

}

CPacketDriverLayer::~CPacketDriverLayer()
{

}

BOOL CPacketDriverLayer::Send( unsigned char* ppayload, int nlength)
{
	
	LPADAPTER  lpAdapter = 0;
	LPPACKET   lpPacket;
	DWORD      dwErrorCode;
	BOOL bSuccess=TRUE;
	CString str;

	lpAdapter =  PacketOpenAdapter(m_adapterName);	
	
	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
	{
		dwErrorCode=GetLastError();
		str.Format("Unable to open the driver, Error Code : %d",dwErrorCode);
		AfxMessageBox(str); 		
		bSuccess=FALSE;
	}

	if((lpPacket = PacketAllocatePacket())==NULL){
		AfxMessageBox("Error:failed to allocate the LPPACKET structure.");
		bSuccess=FALSE;
	}
	PacketInitPacket(lpPacket,ppayload,nlength);
	
	if(PacketSetNumWrites(lpAdapter,1)==FALSE){
		AfxMessageBox("warning: Unable to send more than one packet in a single write!");
		bSuccess=FALSE;
	}
	
	if(PacketSendPacket(lpAdapter,lpPacket,TRUE)==FALSE){
		AfxMessageBox("Error sending the packets!");
		bSuccess=FALSE;
	}
	
	PacketFreePacket(lpPacket);	    
	PacketCloseAdapter(lpAdapter);
	
	
	return bSuccess;

}

BOOL CPacketDriverLayer::Receive()
{
	
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct pcap_pkthdr *header;
	int res;
	const unsigned char *pkt_data;
	CString str;

	BOOL bSuccess=FALSE;
		
	if ( (fp= pcap_open(m_adapterName,
                            100 /*snaplen*/,
                            PCAP_OPENFLAG_PROMISCUOUS /*flags*/,
                            20 /*read timeout*/,
                            NULL /* remote authentication */,
                            errbuf)
                            ) == NULL){
        
            AfxMessageBox("\nError opening adapter\n");
            return FALSE;
        }

	while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {
        if(res == 0)            
            continue;
		
		memcpy(m_packetData,pkt_data,PACKET_MAX_SIZE);   
		
        if(mp_aUpperLayer[0]->Receive(m_packetData))
			bSuccess=TRUE;
		
	}
	
	return bSuccess;
}

void CPacketDriverLayer::SetAdapterName(char* name)
{
		sprintf(m_adapterName,"%s",name);			//GetMacAddr()메소드의 경우에 따라 다르게 설정				
}