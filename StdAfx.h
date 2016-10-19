// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1540D9EA_2446_498E_AB46_3EA3820EA959__INCLUDED_)
#define AFX_STDAFX_H__1540D9EA_2446_498E_AB46_3EA3820EA959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include "windows.h"
#endif // _AFX_NO_AFXCMN_SUPPORT

#define MAX_LAYER_NUMBER		0xff

#define SIZE_UDP_HEADER 8
#define SIZE_UDP_DATA 1458

#define SIZE_IP_HEADER	20
#define	SIZE_IP_DATA 1466

#define SIZE_ICMP_HEADER	8
#define	SIZE_ICMP_DATA 32

#define SIZE_ARP_HEADER	28
#define SIZE_ARP_DATA 1458

#define SIZE_ETHERNET_HEADER 14
#define SIZE_ETHERNET_DATA 1486

#define PACKET_MAX_SIZE 1500

#define TYPE_RIP_REQUEST 0x01
#define TYPE_RIP_RESPONSE 0x02
#define TYPE_RIP_VERSION 0x02
#define TYPE_RIP_ADDRFAMILY 0x0200
#define TYPE_RIP_ROUTETAG 0x0000

#define TYPE_IP_VERLEN 0x45
#define TYPE_IP_HEADERLENG 0x14
#define TYPE_IP_TOS 0x00
#define TYPE_IP_FLAG 0x0
#define TYPE_IP_FRAGOFF 0x0
#define TYPE_IP_TTL 0x80
#define TYPE_IP_UDPTYPE 0x11
#define TYPE_IP_ICMP 0x01

#define TYPE_ETHERNET_ARPTYPE 0x0608
#define TYPE_ETHERNET_IPTYPE 0x0008

#define TYPE_ARP_HARD_TYPE 0x0100
#define TYPE_ARP_PROT_TYPE	0x0008
#define TYPE_ARP_REPLY	0x0200
#define TYPE_ARP_REQUEST	0x0100
#define TYPE_ARP_NOTHING	0x0000

#define UDP_PORT 0x0802

#define MAX_ARP_CACHE 20
#define MAX_ARP_PROXY 20
#define MAX_ADAPTER_INFO 10
#define MAX_QUEUE 20

#define MAX_ROUTING_TABLE 25

typedef struct _MAC_ADDR{			//Application에서 주소저장을 위한 구조체	
	
	char MacAddrDescrip[200];
	char MacAddrName[200];
	unsigned char *IpAddress;	
	unsigned char MacAddress[6];			
	
}MAC_ADDR, *ptrMAC_ADDR;

typedef struct _ARP_CACHE{
	unsigned char	arp_ip[4];
	unsigned char	arp_mac[6];
	char			arp_NIC[200];
	BOOL			arp_state;
	unsigned short	arp_cache_ttl;
}ARP_CACHE, *ptrARP_CACHE;


typedef struct _ARP_PROXY{
		char			MacAddrDescrip[200];
		unsigned char	IpAddress[4];
		int				DescriptoinSize;
}ARP_PROXY, *ptrARP_PROXY;



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1540D9EA_2446_498E_AB46_3EA3820EA959__INCLUDED_)
