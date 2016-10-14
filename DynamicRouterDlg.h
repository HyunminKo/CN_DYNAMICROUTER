// DynamicRouterDlg.h : header file
//

#if !defined(AFX_DynamicRouterDLG_H__7588016B_D61F_471F_959C_E4F05F73D567__INCLUDED_)
#define AFX_DynamicRouterDLG_H__7588016B_D61F_471F_959C_E4F05F73D567__INCLUDED_

#include "pcap.h"
#include "ntddndis.h"
#include "packet32.h"
#include "AddStaticTable.h"
#include "ArpLayer.h"
#include "IpLayer.h"
#include "EthernetLayer.h"
#include "PacketDriverLayer.h"
#include "BaseLayer.h"
#include "LayerManager.h"
#include "ProxyARP.h"
#include "MyThread.h"
#include "RoutingTable.h"
#include "RIP.h"
#include "UDP.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDynamicRouterDlg dialog

class CDynamicRouterDlg : public CDialog,
	public CBaseLayer
{	
// Construction
public:
	void RefreshRoutingTable();
	CBaseLayer* FindCompatibleLayer(int layerNum,char *NICDescription);
	int m_nSelectedItem;
	int m_nadapterNum;
	int m_ntemp;			//Thread를 장치숫자대로 생성할때 이용되는 변수 
	void SetDlgState(int state);
	int  FindAvailableStaticTableNum();
	void RefreshCacheTable();
	CDynamicRouterDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL Send( unsigned char* ppayload, int nlength,unsigned char *dstAddr, unsigned char *deviceDescriptor);
	void GetMacAddr();
	void OnOK();
	LPMAC_ADDR m_Addr[MAX_ADAPTER_INFO];
	MAC_ADDR m_srcAddr;
	char* MacAddrNtoS(unsigned char *MAC);
	char* IpAddrNtoS(unsigned char *IP);
	unsigned char* IpAddrStoN(char* IpString);
	unsigned char* MacAddrStoN(char * MacString);
	CLayerManager m_LayerMgr;
	CMyThread *m_pRecieving[MAX_ADAPTER_INFO];
	CMyThread *m_pTimeToLive;
	CMyThread *m_pRIPThread;
	
	BOOL m_bSetButton;
	int m_nSetButtonFlag;
	CRoutingTable::LPROUTING_TABLE *m_routingTable;
	LPARP_CACHE m_ArpCache[MAX_ARP_CACHE];
	LPARP_PROXY m_ArpProxy[MAX_ARP_PROXY];

	enum{
			SET_STATUS_START,
			SET_STATUS_STOP,
			SET_STATUS_RESUME
	};

	
// Dialog Data
	//{{AFX_DATA(CDynamicRouterDlg)
	enum { IDD = IDD_DynamicRouter_DIALOG };
	CListCtrl	m_listctrlProxy;
	CListCtrl	m_listctrlRouting;
	CListCtrl	m_listctrlCache;
	CComboBox	m_cbNIC;
	CString	m_edIPAddr;
	CString	m_edMACAddr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicRouterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDynamicRouterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboNic();
	afx_msg void OnButtonRoutingAdd();
	afx_msg void OnButtonProxyAdd();
	afx_msg void OnButtonProxyDelete();
	afx_msg void OnButtonCacheDelete();
	afx_msg void OnButtonRoutingDelete();
	afx_msg void OnButtonEnd();
	afx_msg void OnButtonSet();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonRoutingAllclear();
	afx_msg void OnItemchangedListRoutingTable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	afx_msg void OnButtonOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DynamicRouterDLG_H__7588016B_D61F_471F_959C_E4F05F73D567__INCLUDED_)
