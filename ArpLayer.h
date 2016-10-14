// ArpLayer.h: interface for the CArpLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "BaseLayer.h"
#include "EthernetLayer.h"
#include "DynamicRouter.h"


#if !defined(AFX_ARPLAYER_H__3B3C44A9_2583_4870_A80D_6CCAC3F34578__INCLUDED_)
#define AFX_ARPLAYER_H__3B3C44A9_2583_4870_A80D_6CCAC3F34578__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArpLayer 
	:public CBaseLayer 
{
public:
	BOOL FindCompatibleProxy(unsigned char *ipAddr);
	

	typedef struct _ARP{
		unsigned short	arp_hardType;
		unsigned short	arp_protType;
		unsigned char	arp_hardSize;
		unsigned char	arp_protSize;
		unsigned short	arp_op;
		unsigned char	arp_senderEthernetAddr[6];
		unsigned char	arp_senderIPAddr[4];
		unsigned char	arp_targetEthernetAddr[6];
		unsigned char	arp_targetIPAddr[4];
		unsigned char	arp_data[SIZE_ARP_DATA];
	}ARP, *LPARP;

	typedef struct _QUEUE{	
		unsigned char queue_ip[4];
		unsigned char queue_data[SIZE_ARP_DATA];
		int			  queue_dataSize;
	}QUEUE;

	QUEUE m_queue[MAX_QUEUE];
	int rear;
	int front;
	QUEUE deleteQueue();
	void addQueue(QUEUE elem);
	int is_full();
	int is_empty();

	CWnd *m_appWnd;
	char m_NICDescription[200];
	int FindAvailableCacheNum();
	int FindCompatibleCache(unsigned char *dstIPAddr);
	
	void SetAddress(unsigned char *srcIPAddr,unsigned char *srcMacAddr,char *NICDescription);
	BOOL Receive(unsigned char *ppayload);
	BOOL Send( unsigned char* ppayload, int nlength, unsigned char *dstIPAddr, BOOL bRIP);
	
	CArpLayer(char *pName);
	virtual ~CArpLayer();	

	ARP m_header;
	LPARP_CACHE *arpCache;
	LPARP_PROXY *arpProxy;	

};

#endif // !defined(AFX_ARPLAYER_H__3B3C44A9_2583_4870_A80D_6CCAC3F34578__INCLUDED_)
