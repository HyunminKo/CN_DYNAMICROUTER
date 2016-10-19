// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "BaseLayer.h"
//#include "ArpLayer.h"
//#include "IpLayer.h"

#if !defined(AFX_ETHERNETLAYER_H__54BAAA83_46D1_41EB_B3FD_5CEF274B34C5__INCLUDED_)
#define AFX_ETHERNETLAYER_H__54BAAA83_46D1_41EB_B3FD_5CEF274B34C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEthernetLayer  
	:public CBaseLayer
{
public:
	BOOL Send( unsigned char* ppayload, int nlength,unsigned char *dstMACaddr,BOOL bArpType);
	BOOL Receive(unsigned char *ppayload);
	void SetSrcMacAddr(unsigned char *srcAddr );
	CEthernetLayer(char* pName);
	virtual ~CEthernetLayer();

	typedef struct _ETHERNET_ADDR{
		union{
			struct {unsigned char e0, e1, e2, e3, e4, e5;}s_un_byte;			
			unsigned char s_ether_addr[6];			
		}S_un;

	}ETHERNET_ADDR, *ptrETHERNET_ADDR;

	typedef struct _ETHERNET{
		ETHERNET_ADDR	enet_desAddr;
		ETHERNET_ADDR	enet_srcAddr;
		unsigned short	enet_frameType;
		unsigned char	enet_data[SIZE_ETHERNET_DATA];
	}ETHERNET, *ptrETHERNET;

	ETHERNET m_header;

};

#endif // !defined(AFX_ETHERNETLAYER_H__54BAAA83_46D1_41EB_B3FD_5CEF274B34C5__INCLUDED_)
