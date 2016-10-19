// IpLayer.h: interface for the CIpLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "BaseLayer.h"
#include "DynamicRouterDlg.h"
#include "ArpLayer.h"
#include "RoutingTable.h"

#if !defined(AFX_IPLAYER_H__C3515765_1379_45C4_8E81_5B994BD9FF7B__INCLUDED_)
#define AFX_IPLAYER_H__C3515765_1379_45C4_8E81_5B994BD9FF7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIpLayer  
	:public CBaseLayer
{
public:
	typedef struct _IP_ADDR{
		union{
			struct{unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
			struct{unsigned char s_w1,s_w2;}S_un_w;
			unsigned long S_addr;
			unsigned char S_ip_addr[4];

		};
	}IP_ADDR, *ptrIP_ADDR;

	typedef struct _IP{		
		unsigned char	ip_verlen;
		unsigned char	ip_tos;
		unsigned short	ip_len;
		unsigned short	ip_id;
		unsigned short	ip_fragoff;
		unsigned char	ip_ttl;
		unsigned char	ip_proto;
		unsigned short	ip_cksum;
		IP_ADDR			ip_source;
		IP_ADDR			ip_destination;		
		unsigned char	ip_data[SIZE_IP_DATA];
	}IP, *ptrIP;

	typedef struct _ICMP{
		unsigned char	icmp_type;
		unsigned char	icmp_code;
		unsigned short	icmp_cksum;
		unsigned short	icmp_id;
		unsigned short	icmp_seq;
		unsigned char	icmp_data[SIZE_ICMP_DATA];
	}ICMP, *ptrICMP;

	BOOL IsCorrectCheckSum(ptrIP header);
	void MakeCheckSum();
	int ICMP_checksum(u_short* data,int len);
	unsigned short m_ipFragment;
	unsigned char* IPNetmask(unsigned char *dstIP, unsigned char *nestmask);
	int FindDstOfTable(unsigned char *dstIP);
	BOOL Send( unsigned char* ppayload, int nlength ,unsigned char *dstAddr, BOOL bRIP);
	BOOL Receive(unsigned char *ppayload);
	void SetSrcIPAddr(unsigned char *srcIPAddr,char *NICDescription);
	char m_NICDescription[200];
	CWnd *m_appWnd;
	CIpLayer(char* pName);
	virtual ~CIpLayer();

	
	IP m_header;
	CRoutingTable::ptrROUTING_TABLE *m_routingTable;

};

#endif // !defined(AFX_IPLAYER_H__C3515765_1379_45C4_8E81_5B994BD9FF7B__INCLUDED_)
