#if !defined(AFX_ADDSTATICTABLE_H__B3AF6E5E_DB9F_4438_BBB1_5E1B367660AA__INCLUDED_)
#define AFX_ADDSTATICTABLE_H__B3AF6E5E_DB9F_4438_BBB1_5E1B367660AA__INCLUDED_

#include "DynamicRouter.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddStaticTable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddStaticTable dialog

class CAddStaticTable : public CDialog
{
// Construction
public:
	int FindAvailableStaticTableNum();
	CAddStaticTable(CWnd* pParent = NULL);   // standard constructor
	void OnOK();

// Dialog Data
	//{{AFX_DATA(CAddStaticTable)
	enum { IDD = IDD_DIALOG_ADD_ROUTE };
	CComboBox	m_cbInterface;
	BOOL	m_bCheckGateway;
	BOOL	m_bCheckHost;
	BOOL	m_bCheckUp;
	CString	m_edDestination;
	CString	m_edGateway;
	CString	m_edNetmask;
	CString	m_edMetric;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddStaticTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddStaticTable)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDSTATICTABLE_H__B3AF6E5E_DB9F_4438_BBB1_5E1B367660AA__INCLUDED_)
