// RIP.h: interface for the CRIP class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseLayer.h"
#include "RoutingTable.h"
#include "DynamicRouterDlg.h"

#if !defined(AFX_RIP_H__D28B0EF5_2697_4791_A728_9076AB4AD7A4__INCLUDED_)
#define AFX_RIP_H__D28B0EF5_2697_4791_A728_9076AB4AD7A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRIP  
	:public CBaseLayer
{
public:
	void TriggeredUpdate(BOOL *update, char* NICDescription);
	int FindRedundancy(unsigned char *ipAddr);
	int FindAvailableStaticTableNum();
	CRIP(char* pName);
	virtual ~CRIP();
	BOOL Send(int RIPtype);
	BOOL Receive(unsigned char *ppayload, char* NICDescription, unsigned char* dstIPAddr);
	CRoutingTable::ptrROUTING_TABLE *m_routingTable;

	typedef struct _IP_ADDR{
		union{
			struct{unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
			struct{unsigned char s_w1,s_w2;}S_un_w;
			unsigned long S_addr;
			unsigned char S_ip_addr[4];

		};
	}IP_ADDR, *ptrIP_ADDR;

	typedef struct _RIP_ADDR_INFO{
		unsigned short		rm_addrfamily;
		unsigned short		rm_routetag;
		IP_ADDR				rm_ip;
		IP_ADDR				rm_subnetmask;
		IP_ADDR				rm_nexthopip;
		unsigned int		rm_metric;

	}RIP_ADDR_INFO, *ptrRIP_ADDR_INFO;
	
	typedef struct _RIP{
		unsigned char		rm_cmd;
		unsigned char		rm_version;
		unsigned short		rm_unused;		
		RIP_ADDR_INFO		rm_addr[25];
	}RIP, *ptrRIP;

	RIP m_header;
};

#endif // !defined(AFX_RIP_H__D28B0EF5_2697_4791_A728_9076AB4AD7A4__INCLUDED_)
//add Text