// DynamicRouter.h : main header file for the DynamicRouter application
//

#if !defined(AFX_DynamicRouter_H__C5329E10_E6E8_4E66_99DF_C633D0B26167__INCLUDED_)
#define AFX_DynamicRouter_H__C5329E10_E6E8_4E66_99DF_C633D0B26167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDynamicRouterApp:
// See DynamicRouter.cpp for the implementation of this class
//

class CDynamicRouterApp : public CWinApp
{
public:
	CDynamicRouterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicRouterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDynamicRouterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DynamicRouter_H__C5329E10_E6E8_4E66_99DF_C633D0B26167__INCLUDED_)
