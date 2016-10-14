// AddStaticTable.cpp : implementation file
//

#include "stdafx.h"
#include "DynamicRouter.h"
#include "AddStaticTable.h"
#include "DynamicRouterDlg.h"
#include "IpLayer.h"
#include "RoutingTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddStaticTable dialog


CAddStaticTable::CAddStaticTable(CWnd* pParent /*=NULL*/)
	: CDialog(CAddStaticTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddStaticTable)
	m_bCheckGateway = FALSE;
	m_bCheckHost = FALSE;
	m_bCheckUp = FALSE;
	m_edDestination = _T("");
	m_edGateway = _T("");
	m_edNetmask = _T("");
	m_edMetric = _T("");
	//}}AFX_DATA_INIT
}


void CAddStaticTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddStaticTable)
	DDX_Control(pDX, IDC_COMBO_INTERFACE, m_cbInterface);
	DDX_Check(pDX, IDC_CHECK_GATEWAY, m_bCheckGateway);
	DDX_Check(pDX, IDC_CHECK_HOST, m_bCheckHost);
	DDX_Check(pDX, IDC_CHECK_UP, m_bCheckUp);
	DDX_Text(pDX, IDC_EDIT_DESTINATION, m_edDestination);
	DDX_Text(pDX, IDC_EDIT_GATEWAY, m_edGateway);
	DDX_Text(pDX, IDC_EDIT_NETMASK, m_edNetmask);
	DDX_Text(pDX, IDC_EDIT_METRIC, m_edMetric);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddStaticTable, CDialog)
	//{{AFX_MSG_MAP(CAddStaticTable)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddStaticTable message handlers

BOOL CAddStaticTable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData(TRUE);
	// TODO: Add extra initialization here
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)AfxGetMainWnd();
	LPMAC_ADDR *temp=pDlg->m_Addr;

	for(int i=0;i<MAX_ADAPTER_INFO;i++){
		if(temp[i])
			m_cbInterface.AddString(temp[i]->MacAddrDescrip);
	}
	
	m_cbInterface.SetCurSel(0);
	m_edDestination="200.200.0.0";
	m_edNetmask="255.255.255.0";
	m_edGateway="200.200.0.0";
	m_bCheckUp=TRUE;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAddStaticTable::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)AfxGetMainWnd();
	int nCount=pDlg->m_listctrlRouting.GetItemCount();	
	int availableNum=FindAvailableStaticTableNum();
	LV_ITEM lvItem;	
	CRoutingTable::LPROUTING_TABLE *routingTable=((CRoutingTable*)pDlg->m_LayerMgr.GetLayer("RoutingTable"))->m_routingTable;
	CString strCount, strCheck, temp;
	char check[10]={0x00}, deviceDescriptor[200]={0x00};

	m_cbInterface.GetLBText(m_cbInterface.GetCurSel(),strCount);

	if( strCount.IsEmpty() || m_edDestination.IsEmpty() || m_edGateway.IsEmpty() || m_edNetmask.IsEmpty() || m_edMetric.IsEmpty()){
		AfxMessageBox("뭐여 빈칸 있잔아 후딱 다시써-_-");
		return;
	}
	
	if(m_bCheckUp){							//체크박스의 내용을 확인
		sprintf(check,"%s","U");
	}
	if(m_bCheckGateway){
		sprintf(check,"%s%s",check,"G");
	}
	if(m_bCheckHost){
		sprintf(check,"%s%s",check,"H");
	}
	strCheck.Format("%s",check);

	if( availableNum != -1 ){
		
		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=0;	
		lvItem.pszText=(LPSTR)(LPCSTR)m_edDestination;
		pDlg->m_listctrlRouting.InsertItem(&lvItem);

		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=1;	
		lvItem.pszText=(LPSTR)(LPCSTR)m_edNetmask;
		pDlg->m_listctrlRouting.SetItem(&lvItem);

		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=2;	
		lvItem.pszText=(LPSTR)(LPCSTR)m_edGateway;
		pDlg->m_listctrlRouting.SetItem(&lvItem);

		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=3;	
		lvItem.pszText=(LPSTR)(LPCSTR)strCheck;
		pDlg->m_listctrlRouting.SetItem(&lvItem);

		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=4;	
		lvItem.pszText=(LPSTR)(LPCSTR)strCount;
		pDlg->m_listctrlRouting.SetItem(&lvItem);

		lvItem.mask=LVIF_TEXT;
		lvItem.iItem=nCount;
		lvItem.iSubItem=5;	
		lvItem.pszText=(LPSTR)(LPCSTR)m_edMetric;
		pDlg->m_listctrlRouting.SetItem(&lvItem);
		

		routingTable[availableNum]=(CRoutingTable::LPROUTING_TABLE)malloc(sizeof(CRoutingTable::ROUTING_TABLE));
		memcpy(routingTable[availableNum]->destination.S_ip_addr,pDlg->IpAddrStoN((LPSTR)(LPCSTR)m_edDestination),4);
		memcpy(routingTable[availableNum]->gateway.S_ip_addr,pDlg->IpAddrStoN((LPSTR)(LPCSTR)m_edGateway),4);
		memcpy(routingTable[availableNum]->netmask.S_ip_addr,pDlg->IpAddrStoN((LPSTR)(LPCSTR)m_edNetmask),4);
		sprintf(routingTable[availableNum]->flag,"%s",(LPSTR)(LPCSTR)strCheck);
		sprintf(routingTable[availableNum]->device_descripter,"%s",((LPSTR)(LPCSTR)strCount));				
		routingTable[availableNum]->metric=strtol(((LPSTR)(LPCSTR)m_edMetric),NULL,10);	
		routingTable[availableNum]->cnt=0;
	}
	else
		AfxMessageBox("테이블이 꽉찼다는 ㄷㄷㄷ;;");

	this->EndDialog(0);		
	
	UpdateData(FALSE);
}

void CAddStaticTable::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	this->EndDialog(0);		
	
}

int CAddStaticTable::FindAvailableStaticTableNum()
{ 
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)AfxGetMainWnd();
	CRoutingTable::LPROUTING_TABLE *staticTable=((CRoutingTable*)pDlg->m_LayerMgr.GetLayer("RoutingTable"))->m_routingTable;
	
	for( int i=0 ; i<MAX_ROUTING_TABLE ; i++ ){
		if( !staticTable[i] )
			return i;		//첫번째 NULL포인터의 위치		
	}
	return -1;	
}

void CAddStaticTable::OnOK()
{
	OnButtonAdd();
}
