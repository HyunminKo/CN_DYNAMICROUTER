// ProxyARP.cpp : implementation file
//

#include "stdafx.h"
#include "DynamicRouter.h"
#include "ProxyARP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProxyARP dialog


CProxyARP::CProxyARP(CWnd* pParent /*=NULL*/)
	: CDialog(CProxyARP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProxyARP)
	m_edIPAddr = _T("");
	//}}AFX_DATA_INIT
}


void CProxyARP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxyARP)
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_cbDevice);
	DDX_Text(pDX, IDC_EDIT_IPADDRESS, m_edIPAddr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProxyARP, CDialog)
	//{{AFX_MSG_MAP(CProxyARP)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProxyARP message handlers

BOOL CProxyARP::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(TRUE);
	// TODO: Add extra initialization here
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)AfxGetMainWnd();
	LPMAC_ADDR *temp=pDlg->m_Addr;

	for(int i=0;i<MAX_ADAPTER_INFO;i++){
		if(temp[i])
			m_cbDevice.AddString(temp[i]->MacAddrDescrip);
	}
	
	m_cbDevice.SetCurSel(0);
	m_edIPAddr="192.168.0.0";
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CProxyARP::OnOK()
{
	OnButtonOk();
}

void CProxyARP::OnButtonOk() 
{
	// TODO: Add your control notification handler code here
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)AfxGetMainWnd();
	int nCount=pDlg->m_listctrlProxy.GetItemCount();	
	LPARP_PROXY *arpProxy=pDlg->m_ArpProxy;
	int availableNum=FindAvailableProxyNum();
	LV_ITEM lvItem;
	CString strCount;
	
	UpdateData(TRUE);

	m_cbDevice.GetLBText(m_cbDevice.GetCurSel(),strCount);
	
	if( strCount.IsEmpty() || m_edIPAddr.IsEmpty() ){
		AfxMessageBox("필수값이 존재하지 않습니다.");
		return;
	}
	
	if( availableNum != -1 ){
		
		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=0;	
		lvItem.pszText=(LPSTR)(LPCSTR)strCount;
		pDlg->m_listctrlProxy.InsertItem(&lvItem);
		
		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=1;	
		lvItem.pszText=(LPSTR)(LPCSTR)m_edIPAddr;
		pDlg->m_listctrlProxy.SetItem(&lvItem);
		
		arpProxy[availableNum]=(LPARP_PROXY)malloc(sizeof(ARP_PROXY));
		sprintf(arpProxy[availableNum]->MacAddrDescrip,"%s",(LPSTR)(LPCSTR)strCount);
		memcpy(arpProxy[availableNum]->IpAddress,pDlg->IpAddrStoN((LPSTR)(LPCSTR)m_edIPAddr),4);
		arpProxy[availableNum]->DescriptoinSize=strlen(arpProxy[availableNum]->MacAddrDescrip);		
	}
	else
		AfxMessageBox("더이상 Proxy 추가 못한다는 ㄷㄷㄷ;; 테이블이 꽉찼어");
	
	UpdateData(FALSE);
	

	this->EndDialog(0);	
	
}

void CProxyARP::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	this->EndDialog(0);	
	
}

int CProxyARP::FindAvailableProxyNum()
{
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)AfxGetMainWnd();
	LPARP_PROXY *arpProxy=pDlg->m_ArpProxy;

	for(int i=0;i<MAX_ARP_PROXY;i++){
		if(!arpProxy[i])
			return i;		//첫번째 NULL포인터의 위치
		}	
	return -1;
}
