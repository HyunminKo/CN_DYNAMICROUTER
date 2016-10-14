#if !defined(AFX_PROXYARP1_H__8FACCA51_C182_41D2_BB21_77E7411C81D4__INCLUDED_)
#define AFX_PROXYARP1_H__8FACCA51_C182_41D2_BB21_77E7411C81D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProxyARP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxyARP dialog
#include "StdAfx.h"
#include "DynamicRouterDlg.h"

class CProxyARP : public CDialog
{
// Construction
public:
	CProxyARP(CWnd* pParent = NULL);   // standard constructor
	int FindAvailableProxyNum();
	void OnOK();
// Dialog Data
	//{{AFX_DATA(CProxyARP)
	enum { IDD = IDD_DIALOG_PROXY_ARP };
	CComboBox	m_cbDevice;
	CString	m_edIPAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxyARP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProxyARP)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOk();
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYARP1_H__8FACCA51_C182_41D2_BB21_77E7411C81D4__INCLUDED_)
