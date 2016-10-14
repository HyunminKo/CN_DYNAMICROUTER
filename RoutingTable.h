// RoutingTable.h: interface for the CRoutingTable class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseLayer.h"

#if !defined(AFX_ROUTINGTABLE_H__F87CB881_A435_4ED2_B607_650CB77C90FB__INCLUDED_)
#define AFX_ROUTINGTABLE_H__F87CB881_A435_4ED2_B607_650CB77C90FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRoutingTable  
	:public CBaseLayer 
{
public:
	CRoutingTable(char* pName);
	virtual ~CRoutingTable();
	
	typedef struct _IP_ADDR{
		union{
			struct{unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
			struct{unsigned char s_w1,s_w2;}S_un_w;
			unsigned long S_addr;
			unsigned char S_ip_addr[4];

		};
	}IP_ADDR, *LPIP_ADDR;

	typedef struct _ROUTING_TABLE{
		IP_ADDR		destination;
		IP_ADDR		netmask;
		IP_ADDR		gateway;
		char		flag[10];
		char		device_descripter[200];		
		int			metric;
		int			cnt;
	}ROUTING_TABLE, *LPROUTING_TABLE;
	

	LPROUTING_TABLE m_routingTable[MAX_ROUTING_TABLE];

};

#endif // !defined(AFX_ROUTINGTABLE_H__F87CB881_A435_4ED2_B607_650CB77C90FB__INCLUDED_)
