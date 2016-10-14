// PacketDriverLayer.h: interface for the CPacketDriverLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "BaseLayer.h"
#include "pcap.h"
#include "packet32.h"



#if !defined(AFX_PACKETDRIVERLAYER_H__2EA4F99E_2D7B_458A_A9B8_156C5B38B7EE__INCLUDED_)
#define AFX_PACKETDRIVERLAYER_H__2EA4F99E_2D7B_458A_A9B8_156C5B38B7EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPacketDriverLayer  
	:public CBaseLayer
{
public:
	
	CPacketDriverLayer(char* pName);
	virtual ~CPacketDriverLayer();
	
	char m_adapterName[200];
	unsigned char m_packetData[PACKET_MAX_SIZE];	
	BOOL Send( unsigned char* ppayload, int nlength);
	BOOL Receive();
	void SetAdapterName(char* name);

};

#endif // !defined(AFX_PACKETDRIVERLAYER_H__2EA4F99E_2D7B_458A_A9B8_156C5B38B7EE__INCLUDED_)
