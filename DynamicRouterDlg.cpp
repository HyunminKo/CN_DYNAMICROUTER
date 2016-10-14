// DynamicRouterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DynamicRouter.h"
#include "DynamicRouterDlg.h"
#include "afxmt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


UINT ReceivingData(LPVOID lParam);
UINT TimeToLive(LPVOID lParam);
UINT RIPThread(LPVOID lParam);
CCriticalSection g_cs1,g_cs2;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicRouterDlg dialog

CDynamicRouterDlg::CDynamicRouterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicRouterDlg::IDD, pParent) , 
	CBaseLayer( "DynamicRouting" )
{
	//{{AFX_DATA_INIT(CDynamicRouterDlg)
	m_edIPAddr = _T("");
	m_edMACAddr = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for(int i=0;i<MAX_ADAPTER_INFO;i++)
		m_Addr[i]=NULL;

	for(int i=0;i<MAX_ADAPTER_INFO;i++)
		m_pRecieving[i]=NULL;

	for(int i=0;i<MAX_ARP_CACHE;i++)
		m_ArpCache[i]=NULL;	

	for(int i=0;i<MAX_ARP_PROXY;i++)
		m_ArpProxy[i]=NULL;	

	m_bSetButton=FALSE;
	m_nadapterNum=0;
	m_nSetButtonFlag=0;
	m_nSelectedItem=0;
	m_ntemp=0;	
}

void CDynamicRouterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDynamicRouterDlg)
	DDX_Control(pDX, IDC_LIST_PROXY, m_listctrlProxy);
	DDX_Control(pDX, IDC_LIST_ROUTING_TABLE, m_listctrlRouting);
	DDX_Control(pDX, IDC_LIST_CACHE_TABLE, m_listctrlCache);
	DDX_Control(pDX, IDC_COMBO_NIC, m_cbNIC);
	DDX_Text(pDX, IDC_EDIT_IPADDR, m_edIPAddr);
	DDX_Text(pDX, IDC_EDIT_MACADDR, m_edMACAddr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDynamicRouterDlg, CDialog)
	//{{AFX_MSG_MAP(CDynamicRouterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_NIC, OnSelchangeComboNic)
	ON_BN_CLICKED(IDC_BUTTON_ROUTING_ADD, OnButtonRoutingAdd)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_ADD, OnButtonProxyAdd)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_DELETE, OnButtonProxyDelete)
	ON_BN_CLICKED(IDC_BUTTON_CACHE_DELETE, OnButtonCacheDelete)
	ON_BN_CLICKED(IDC_BUTTON_ROUTING_DELETE, OnButtonRoutingDelete)
	ON_BN_CLICKED(IDC_BUTTON_END, OnButtonEnd)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_ROUTING_ALLCLEAR, OnButtonRoutingAllclear)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROUTING_TABLE, OnItemchangedListRoutingTable)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicRouterDlg message handlers

BOOL CDynamicRouterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	GetMacAddr();

	char *strIP="IP", *strARP="Arp", *strEthernet="Ethernet", *strPacket="PacketDriver", *strUDP="UDP", *ptr=NULL;	

	m_LayerMgr.AddLayer(new CRIP("RIP"));	
	m_LayerMgr.AddLayer(new CRoutingTable("RoutingTable"));	
	m_LayerMgr.AddLayer(this);
	
	for(int n=1; n <= m_nadapterNum ; n++ ){				
		
		ptr=(char*)malloc(sizeof(char)*20);
		sprintf(ptr,"%s%d",strPacket,n);
		m_LayerMgr.AddLayer(new CPacketDriverLayer(ptr));		
		
		ptr=(char*)malloc(sizeof(char)*20);
		sprintf(ptr,"%s%d",strEthernet,n);
		m_LayerMgr.AddLayer(new CEthernetLayer(ptr));			
		
		ptr=(char*)malloc(sizeof(char)*20);
		sprintf(ptr,"%s%d",strARP,n);
		m_LayerMgr.AddLayer(new CArpLayer(ptr));
		((CArpLayer*)m_LayerMgr.GetLayer(ptr))->m_appWnd=this;		//ARP계층에서 UI를 위한 포인터 설정			
		((CArpLayer*)m_LayerMgr.GetLayer(ptr))->arpCache=m_ArpCache;
		((CArpLayer*)m_LayerMgr.GetLayer(ptr))->arpProxy=m_ArpProxy;
		
		
		ptr=(char*)malloc(sizeof(char)*20);
		sprintf(ptr,"%s%d",strIP,n);
		m_LayerMgr.AddLayer(new CIpLayer(ptr));					   //각계층을 생성해서 연결	
		((CIpLayer*)m_LayerMgr.GetLayer(ptr))->m_appWnd=this;		//ARP계층에서 UI를 위한 포인터 설정			
		((CIpLayer*)m_LayerMgr.GetLayer(ptr))->m_routingTable=((CRoutingTable*)m_LayerMgr.GetLayer("RoutingTable"))->m_routingTable;
		
		ptr=(char*)malloc(sizeof(char)*20);
		sprintf(ptr,"%s%d",strUDP,n);
		m_LayerMgr.AddLayer(new CUDP(ptr));
		
		ptr=(char*)malloc(sizeof(char)*150);
		sprintf(ptr,"%s%d ( *%s%d ( *%s%d  +%s%d ( *%s%d ( *RIP ( *DynamicRouting ) ) -%s%d ) ) )",
			strPacket,n,strEthernet,n,strARP,n,strIP,n,strUDP,n,strARP,n);
		
		m_LayerMgr.ConnectLayers(ptr);		
	}
	
	m_routingTable=((CRoutingTable*)m_LayerMgr.GetLayer("RoutingTable"))->m_routingTable;
	((CRIP*)m_LayerMgr.GetLayer("RIP"))->m_routingTable=((CRoutingTable*)m_LayerMgr.GetLayer("RoutingTable"))->m_routingTable;;

	LV_COLUMN lvcolumn;
	char *list1[6]={"Destination","NetMask","GateWay","Flag","Interface","Metric"};
	int nWidth1[6]={100,100,100,50,150,50};

	for(int i=0;i<6;i++){
		lvcolumn.mask=LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt=LVCFMT_CENTER;
		lvcolumn.pszText=list1[i];
		lvcolumn.iSubItem=i;
		lvcolumn.cx=nWidth1[i];
		m_listctrlRouting.InsertColumn(i,&lvcolumn);		
	}
	m_listctrlRouting.SetExtendedStyle(m_listctrlRouting.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	char *list2[4]={"IP Address","Etherent Address","Interface","Flag"};
	int nWidth2[4]={90,130,150,80};

	for(int i=0;i<4;i++){
		lvcolumn.mask=LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt=LVCFMT_CENTER;
		lvcolumn.pszText=list2[i];
		lvcolumn.iSubItem=i;
		lvcolumn.cx=nWidth2[i];
		m_listctrlCache.InsertColumn(i,&lvcolumn);		
	}
	m_listctrlCache.SetExtendedStyle(m_listctrlCache.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	char *list3[2]={"Device","IP Address"};
	int nWidth3[2]={300,150};

	for(int i=0;i<2;i++){
		lvcolumn.mask=LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt=LVCFMT_CENTER;
		lvcolumn.pszText=list3[i];
		lvcolumn.iSubItem=i;
		lvcolumn.cx=nWidth3[i];
		m_listctrlProxy.InsertColumn(i,&lvcolumn);		
	}
	m_listctrlProxy.SetExtendedStyle(m_listctrlProxy.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDynamicRouterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDynamicRouterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDynamicRouterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CDynamicRouterDlg::GetMacAddr()
{
	UpdateData(TRUE);
	LPADAPTER	lpAdapter = 0;
	int			i=0,n=0;	
	char		AdapterName[8000];
	char		*temp,*temp1;
	int			AdapterNum=0;
	ULONG		AdapterLength;
	PPACKET_OID_DATA  OidData;
	BOOLEAN		Status;	
	
	AdapterLength = sizeof(AdapterName);
	
	if(PacketGetAdapterNames(AdapterName,&AdapterLength)==FALSE){
		AfxMessageBox("NIC 못찾았어-_-");
		return ;
	}	
		  
	AdapterNum=i;
	
	temp=AdapterName;
	temp1=AdapterName;

	while ((*temp!='\0')||(*(temp-1)!='\0'))
	{
		if (*temp=='\0')						//검색된 어댑터이름을 배열에 저장
		{
			m_Addr[i]=(LPMAC_ADDR)malloc(sizeof(MAC_ADDR));
			sprintf(m_Addr[i]->MacAddrName,"%s",temp1);
			temp1=temp+1;
			i++;
		}
		temp++;
	}

	AdapterNum=i;
	OidData =(PPACKET_OID_DATA) malloc(6 + sizeof(PACKET_OID_DATA));
	OidData->Oid = OID_802_3_CURRENT_ADDRESS;
	OidData->Length = 6;	

	CString str;

	for(i=0;i<AdapterNum;i++){					//검색된 맥주소를 배열에 저장 
		ZeroMemory(OidData->Data, 6);
		lpAdapter=PacketOpenAdapter(m_Addr[i]->MacAddrName);	
		Status = PacketRequest(lpAdapter, FALSE, OidData);	
		if(Status){
			memcpy(m_Addr[i]->MacAddress,OidData->Data,6);			
		}
		else{									//유효하지 않은 어댑터의 맥주소는 0으로 초기화
			m_Addr[i]->MacAddress[0]=0;
			m_Addr[i]->MacAddress[1]=0;
			m_Addr[i]->MacAddress[2]=0;
			m_Addr[i]->MacAddress[3]=0;
			m_Addr[i]->MacAddress[4]=0;
			m_Addr[i]->MacAddress[5]=0;
		}
		PacketCloseAdapter(lpAdapter);			//어댑터를 닫는다.
	}	

	free(OidData);
	
	pcap_if_t *alldevs;
	pcap_if_t *d;
	struct sockaddr_in *si;
	char errbuf[100];
	char *invalidateIP="0.0.0.0";
	
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
        {
            AfxMessageBox("Error in pcap_findalldevs_ex");
            return ;
        }        
        
	for(d=alldevs; d; d=d->next)				//검색된 어댑터의 description과 ip를 저장
	{			
		if (d->name){			
			temp=d->name;			
			temp+=8;			
			for(i=0;i<AdapterNum;i++){
				if( !memcmp(m_Addr[i]->MacAddrName,temp,strlen(m_Addr[i]->MacAddrName)) ){		
										
					temp=d->description;				//description의 쓸데없는 정보는 빼고 저장
					temp+=17;
					sprintf(m_Addr[i]->MacAddrDescrip,"%d - %s",i,temp);					
														//유효한 어댑터의 경우 IP저장
					if(	(d->flags != PCAP_IF_LOOPBACK) && ( d->addresses != 0 ) ){												
						si=(struct sockaddr_in*)(d->addresses->addr);						
						m_Addr[i]->IpAddress=IpAddrStoN(inet_ntoa(si->sin_addr));						
					}
					else{								//유효하지 않은 어댑터의 경우 IP를 0으로 초기화	
						m_Addr[i]->IpAddress=IpAddrStoN(invalidateIP);						
					}					
				}
			}
		}
		else
			AfxMessageBox("No description available");
	}
	

	for( i=0; i<MAX_ADAPTER_INFO ; i++ ){				//유효하지 않은 장치를 지운다.
		if( m_Addr[i] && !strcmp(MacAddrNtoS(m_Addr[i]->MacAddress),"00:00:00:00:00:00") ){			
			free(m_Addr[i]);
			m_Addr[i]=NULL;			
		}				
	}
	
	for(i=0;i<MAX_ADAPTER_INFO;i++)						//콤보박스에 장치 표시
		if(m_Addr[i]){
			m_cbNIC.AddString(m_Addr[i]->MacAddrDescrip);
			m_nadapterNum++;
		}

	CString toCompare;
	m_cbNIC.SetCurSel(0);
	m_cbNIC.GetLBText(m_cbNIC.GetCurSel(),str);          //이더넷주소배열에서 현재콤보박스의 내용과	
	for(i=0;i<m_cbNIC.GetCount();i++){				     //일치하는 항목을 검색 		     
		toCompare.Format("%s",m_Addr[i]->MacAddrDescrip);		
		if(!str.Compare(toCompare)){			
			break;
		}		
	}
	
	m_edMACAddr.Format("%s",MacAddrNtoS(m_Addr[i]->MacAddress));	//MacAddress 출력		
	m_edIPAddr.Format("%s",IpAddrNtoS(m_Addr[i]->IpAddress));			//IPAddress 출력
	
	
	UpdateData(FALSE);
}

//string으로 입력된 ip를 실제 4바이트로 바꾸어 리턴해준다.
unsigned char* CDynamicRouterDlg::IpAddrStoN(char* IpString)
{
	unsigned char *result=(unsigned char*)malloc(sizeof(char)*4);	
	char *temp=IpString;
	char ip[4];
	int n=0;	
	
	while(*temp!='\0'){	
		while(*temp != '.' && *temp !='\0'){	//'.'단위로 나누어서 문자를 10진수숫자로 바꾼다.
			ip[n++]= *temp;						//숫자로 바뀐값을 변수에 저장해서 리턴
			temp++;
		}
		ip[n]='\0';		
		n=0;
		*result=(unsigned char)strtol(ip,NULL,10);
		result++;
		if(*temp=='\0'){
			result-=4;							//4바이트 
			return result;
		}
		temp++;
	}
	AfxMessageBox("IP주소 바꾸다가 에러 발생-_-;");
	return NULL;
}


unsigned char* CDynamicRouterDlg::MacAddrStoN(char *MacString)
{
	char mac[3];
	unsigned char addrTemp[6];
	unsigned char *Result=(unsigned char*)malloc(sizeof(unsigned char)*6);
	
	for(int n=0,k=0;n<=15;n+=2){                            //입력받은 문자형태의주소를 
		mac[0]=MacString[n];								   //16진수숫자로 변환
		mac[1]=MacString[n+1];
		mac[2]='\0';
		addrTemp[k++]=(BYTE)strtol(mac,NULL,16);	
		n++;			//MacAddress사이에 ':'표시가 있을 경우 ':'를 무시하기 위해 
	}
	
	for(int n=0;n<6;n++)									//6바이트로 변환해서 리턴
		Result[n]=addrTemp[n];

	return Result;
}

//바이트형식의 IP주소를 문자열로 변환
char* CDynamicRouterDlg::IpAddrNtoS(unsigned char *IP)
{
	char *Result=(char*)malloc(sizeof(char)*20);
	sprintf(Result,"%d.%d.%d.%d",IP[0],IP[1],IP[2],IP[3]);
	return Result;
}

//바이트형식의 MAC주소를 문자열로 변환
char* CDynamicRouterDlg::MacAddrNtoS(unsigned char *MAC)
{
	char *Result=(char*)malloc(sizeof(char)*20);
	sprintf(Result,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
	return Result;
}

void CDynamicRouterDlg::OnSelchangeComboNic() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString str,temp;
	int i;
	m_cbNIC.GetLBText(m_cbNIC.GetCurSel(),str);          //이더넷주소배열에서 현재콤보박스의 내용과
	for(i=0;i<m_cbNIC.GetCount();i++){				       //일치하는 항목을 검색 		     
		temp.Format("%s",m_Addr[i]->MacAddrDescrip);
		if(!str.Compare(temp)){			
			break;
		}		
	}
	m_edMACAddr.Format("%s",MacAddrNtoS(m_Addr[i]->MacAddress));	//MacAddress 출력		
	m_edIPAddr.Format("%s",IpAddrNtoS(m_Addr[i]->IpAddress));			//IPAddress 출력
	
	UpdateData(FALSE);
	
}

void CDynamicRouterDlg::OnButtonRoutingAdd() 
{
	// TODO: Add your control notification handler code here

	CAddStaticTable p=new CAddStaticTable();	
	p.DoModal();
	
}

void CDynamicRouterDlg::OnButtonProxyAdd() 
{
	// TODO: Add your control notification handler code here
	CProxyARP p = new CProxyARP();
	p.DoModal();
	
}
void CDynamicRouterDlg::OnOK()
{
	
}

void CDynamicRouterDlg::OnButtonProxyDelete() 
{
	// TODO: Add your control notification handler code here
	
	int i;
	
	for(i=0;i<MAX_ARP_PROXY;i++){
		if(m_ArpProxy[i]){
			free(m_ArpProxy[i]);
			m_ArpProxy[i]=NULL;
		}	
	}
	m_listctrlProxy.DeleteAllItems();	
	
	
}

void CDynamicRouterDlg::OnButtonCacheDelete() 
{
	// TODO: Add your control notification handler code here
	
	int i;
	
	for(i=0;i<MAX_ARP_CACHE;i++){
		if(m_ArpCache[i]){
			free(m_ArpCache[i]);
			m_ArpCache[i]=NULL;
		}	
	}
	m_listctrlCache.DeleteAllItems();	
	
}

void CDynamicRouterDlg::RefreshCacheTable()
{
	
	CString str;
	unsigned char broadCasting[6];
	LV_ITEM lvItem;
	int nCount, i;
		
	for(i=0;i<6;i++)
		broadCasting[i]=0x00;

	m_listctrlCache.DeleteAllItems();

	for(i=0;i<MAX_ARP_CACHE;i++){
		if(m_ArpCache[i]){
			nCount=m_listctrlCache.GetItemCount();			
			str.Format("%s",IpAddrNtoS(m_ArpCache[i]->arp_ip));
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=0;	
			lvItem.pszText=(LPSTR)(LPCSTR)str;
			m_listctrlCache.InsertItem(&lvItem);
			
			if(!memcmp(m_ArpCache[i]->arp_mac,broadCasting,6)){
				str.Format("??:??:??:??:??:??");
			}else{
				str.Format("%s",MacAddrNtoS(m_ArpCache[i]->arp_mac));
			}
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=1;	
			lvItem.pszText=(LPSTR)(LPCSTR)str;
			m_listctrlCache.SetItem(&lvItem);
			
			str.Format("%s",m_ArpCache[i]->arp_NIC);
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=2;	
			lvItem.pszText=(LPSTR)(LPCSTR)str;
			m_listctrlCache.SetItem(&lvItem);			

			str.Format("%s",m_ArpCache[i]->arp_state ? "complete" : "incomplete");
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=3;	
			lvItem.pszText=(LPSTR)(LPCSTR)str;
			m_listctrlCache.SetItem(&lvItem);
			
		}
	}	
}

void CDynamicRouterDlg::RefreshRoutingTable()
{
	LV_ITEM lvItem;	
	int nCount;
	char charMetric[5]={0x00};

	m_listctrlRouting.DeleteAllItems();	

	nCount=m_listctrlRouting.GetItemCount();	
	
	for(int i=MAX_ROUTING_TABLE-1;i>=0;i--){						//출력순서를 맞추기위해서 배열의 끝에서부터 출력
		if(m_routingTable[i]){
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=0;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->destination.S_ip_addr);
			m_listctrlRouting.InsertItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=1;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->netmask.S_ip_addr);;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=2;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->gateway.S_ip_addr);;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=3;	
			lvItem.pszText=m_routingTable[i]->flag;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=4;	
			lvItem.pszText=m_routingTable[i]->device_descripter;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=5;	
			sprintf(charMetric,"%d",m_routingTable[i]->metric);
			lvItem.pszText=charMetric;
			m_listctrlRouting.SetItem(&lvItem);
		}
	}
}

void CDynamicRouterDlg::OnButtonRoutingDelete() 
{
	LV_ITEM lvItem;	
	int nCount=0,i=0;
	char charMetric[5]={0x00};
	CString str;
	BOOL update[MAX_ROUTING_TABLE];
		
	for(i=0;i<MAX_ROUTING_TABLE;i++)
		update[i]=FALSE;
	str=m_listctrlRouting.GetItemText(m_nSelectedItem,0);	
	
	
	for(i=0;i<MAX_ROUTING_TABLE;i++){								//배열에서 일치하는 항목검색
		if( m_routingTable[i] && !memcmp(m_routingTable[i]->destination.S_ip_addr,IpAddrStoN((LPSTR)(LPCSTR)str),4) ){
			update[i]=TRUE;
			m_routingTable[i]->metric=16;
			if(m_bSetButton)										//start버튼이 눌려서 쓰레드가 돌고 있을 경우 
				((CRIP*)mp_UnderLayer)->TriggeredUpdate(update,"다보내셔~~");
			free(m_routingTable[i]);										//해당항목을 삭제
			m_routingTable[i]=NULL;
		}
	}	
	
	m_listctrlRouting.DeleteAllItems();	

	nCount=m_listctrlRouting.GetItemCount();	
	
	for(i=MAX_ROUTING_TABLE-1;i>=0;i--){						//출력순서를 맞추기위해서 배열의 끝에서부터 출력
		if(m_routingTable[i]){
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=0;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->destination.S_ip_addr);
			m_listctrlRouting.InsertItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=1;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->netmask.S_ip_addr);;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=2;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->gateway.S_ip_addr);;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=3;	
			lvItem.pszText=m_routingTable[i]->flag;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=4;	
			lvItem.pszText=m_routingTable[i]->device_descripter;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=5;	
			sprintf(charMetric,"%d",m_routingTable[i]->metric);
			lvItem.pszText=charMetric;
			m_listctrlRouting.SetItem(&lvItem);
		}
	}
}

void CDynamicRouterDlg::OnButtonEnd() 
{
	exit(0);
}

void CDynamicRouterDlg::SetDlgState(int state)
{
	CButton* pSetButton=(CButton*)GetDlgItem(IDC_BUTTON_START);
	switch(state){
		case SET_STATUS_START:
			pSetButton->SetWindowText("Start");
			break;
		case SET_STATUS_STOP:	
			pSetButton->SetWindowText("Stop");
			break;
		case SET_STATUS_RESUME:
			pSetButton->SetWindowText("Resume");
			break;
	}
}

void CDynamicRouterDlg::OnButtonSet() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString str,temp;
	int i;
	
	m_cbNIC.GetLBText(m_cbNIC.GetCurSel(),str);		        //이더넷주소배열에서 현재콤보박스의 내용과
	for(i=0;i<m_cbNIC.GetCount();i++){				        //일치하는 항목을 검색 		     
		temp.Format("%s",m_Addr[i]->MacAddrDescrip);
		if(!str.Compare(temp)){			
			break;
		}		
	}														//그 항목의 주소를 새로 설정한다.
	memcpy(m_Addr[i]->IpAddress,IpAddrStoN((LPSTR)(LPCSTR)m_edIPAddr),4);
	memcpy(m_Addr[i]->MacAddress,MacAddrStoN((LPSTR)(LPCSTR)m_edMACAddr),6);
	
	UpdateData(FALSE);		
}

void CDynamicRouterDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	int i;

	if(m_bSetButton){									//버튼의 상태를 바꿔주고 
		SetDlgState(SET_STATUS_RESUME);					//thread를 멈춰준다.
		m_bSetButton=FALSE;		
		for(i=0;i<MAX_ADAPTER_INFO;i++){
			if(m_pRecieving[i])				
				m_pRecieving[i]->SuspendThread();				
		}
		m_pTimeToLive->SuspendThread();
		m_pRIPThread->SuspendThread();
		return ;
	}	

	SetDlgState(SET_STATUS_STOP);
	m_bSetButton=TRUE;	
	
	if(m_nSetButtonFlag==0){							// start 버튼을 처음눌렀을 경우 thread생성
		m_nSetButtonFlag++;
		
		for(i=0;i<m_nadapterNum;i++){			
				m_pRecieving[i]=(CMyThread*)AfxBeginThread(ReceivingData,this);							
		}
		m_pTimeToLive=(CMyThread*)AfxBeginThread(TimeToLive,this);
		m_pRIPThread=(CMyThread*)AfxBeginThread(RIPThread,this);
	}
	else{												// 처음이 아닌경우 thread  재시작
		for(i=0;i<m_nadapterNum;i++){
			
				m_pRecieving[i]->ResumeThread();						
		}
		m_pTimeToLive->ResumeThread();
		m_pRIPThread->ResumeThread();
	}		
}


//Packet Driver로부터 패킷을 읽어 오는 Thread
UINT ReceivingData(LPVOID lParam)	
{
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)lParam;		
	int i, k=0;	
	char ch[20]={0x0};
	pDlg->m_ntemp++;

	for(i=0;i<MAX_ADAPTER_INFO;i++){
		if(pDlg->m_Addr[i]){			
			k++;		
			if(pDlg->m_ntemp==k){	
				break;
			}
		}
	}			
	
	if(k==0)
		return 0;
	sprintf(ch,"UDP%d",k);
	((CUDP*)pDlg->m_LayerMgr.GetLayer(ch))->SetAddress(pDlg->m_Addr[k-1]->MacAddrDescrip,pDlg->m_Addr[k-1]->IpAddress);

	sprintf(ch,"IP%d",k);
	((CIpLayer*)pDlg->m_LayerMgr.GetLayer(ch))->SetSrcIPAddr(pDlg->m_Addr[k-1]->IpAddress,pDlg->m_Addr[k-1]->MacAddrDescrip);

	sprintf(ch,"Arp%d",k);
	((CArpLayer*)pDlg->m_LayerMgr.GetLayer(ch))->SetAddress(pDlg->m_Addr[k-1]->IpAddress,pDlg->m_Addr[k-1]->MacAddress,pDlg->m_Addr[k-1]->MacAddrDescrip);
	
	sprintf(ch,"Ethernet%d",k);
	((CEthernetLayer*)pDlg->m_LayerMgr.GetLayer(ch))->SetSrcMacAddr(pDlg->m_Addr[k-1]->MacAddress);	
	
	sprintf(ch,"PacketDriver%d",k);	
	((CPacketDriverLayer*)pDlg->m_LayerMgr.GetLayer(ch))->SetAdapterName(pDlg->m_Addr[k-1]->MacAddrName);
	((CPacketDriverLayer*)pDlg->m_LayerMgr.GetLayer(ch))->Receive();
	
	return 0;
}

// ARP CacheTable에서 시간을 체크하여 테이블을 갱신하는 Thread
UINT TimeToLive(LPVOID lParam)
{	
	
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)lParam;	
	LPARP_CACHE *arpCache=pDlg->m_ArpCache;
	
	unsigned char ppayload[3]="hi";
	
	while(1){
	
		g_cs1.Lock();
		for(int i=0;i<MAX_ARP_CACHE;i++)	//CacheTable에서 일치하는 내용 검색
		{							
			if( arpCache[i] && (arpCache[i]->arp_state==TRUE)){		//해당 Table의 상태를 확인
				arpCache[i]->arp_cache_ttl--;						//유효한 Table이면 ttl값을 감소(1초마다)
				if(arpCache[i]->arp_cache_ttl==0){					//ttl값이 0이면 해당 테이블을 갱신
					arpCache[i]->arp_state=FALSE;
					arpCache[i]->arp_cache_ttl=10;
					pDlg->Send(ppayload,0,arpCache[i]->arp_ip,(unsigned char*)arpCache[i]->arp_NIC);		//arp request를 보낸다.					
					pDlg->RefreshCacheTable();
				}
			}

			if( arpCache[i] && (arpCache[i]->arp_state==FALSE)){	//해당 Table의 상태를 확인
				arpCache[i]->arp_cache_ttl--;						//유효하지 않은값이라면
				if(arpCache[i]->arp_cache_ttl==1){					//1초전에 request를 한번 보낸다.
					pDlg->Send(ppayload,0,arpCache[i]->arp_ip,(unsigned char*)arpCache[i]->arp_NIC);
					pDlg->RefreshCacheTable();
				}
				if(arpCache[i]->arp_cache_ttl==0){					//일정시간이 지난후 Table삭제					
					free(arpCache[i]);
					arpCache[i]=NULL;
					pDlg->RefreshCacheTable();
				}
			}
		}		
		g_cs1.Unlock();
		Sleep(5000);
	}	
	return 0;
}

// rip Thread
UINT RIPThread(LPVOID lParam)
{	
	CDynamicRouterDlg *pDlg=(CDynamicRouterDlg*)lParam ;	
	BOOL update[MAX_ROUTING_TABLE] ;		// triggered 업데이트를 위한 테이블
	int cnt=0, i=0 ;

	for(i=0;i<MAX_ROUTING_TABLE;i++)
		update[i]=FALSE;

	((CRIP*)pDlg->m_LayerMgr.GetLayer("RIP"))->Send(1);			// initial request	

	while(1){		

		g_cs2.Lock();
		((CRIP*)pDlg->m_LayerMgr.GetLayer("RIP"))->Send(2);		// regular response		

		for(i=0;i<MAX_ROUTING_TABLE;i++){						
			if(pDlg->m_routingTable[i])
			{
				// 일반적인 경우
				if( (pDlg->m_routingTable[i]->metric > 1) && (pDlg->m_routingTable[i]->metric < 16) )
					pDlg->m_routingTable[i]->cnt++;				// 업데이트 여부를 확인하기 위한 count 증가				

				// metric이 16일 경우
				if( pDlg->m_routingTable[i]->metric == 16 ){	
					pDlg->m_routingTable[i]->cnt++;				// 30초에 한번씩 카운트를 증가	
					if(pDlg->m_routingTable[i]->cnt >= 4){		// 카운트가 4일 경우 완전히 삭제(120초 garbage collection)						
						free(pDlg->m_routingTable[i]);
						pDlg->m_routingTable[i]=NULL;						
					}
				}

				// count가 6이상인 경우 (180초 동안 업데이트가 되지 않은 상태)
				if( pDlg->m_routingTable[i] && (pDlg->m_routingTable[i]->metric < 16) && (pDlg->m_routingTable[i]->cnt > 6) ){
					pDlg->m_routingTable[i]->metric=16;		// 무효한 연결
					pDlg->m_routingTable[i]->cnt=0;			// garbage collection을 위한 카운터 init
					update[i]=TRUE;	
				}
			}		
		}
		pDlg->RefreshRoutingTable();

		((CRIP*)pDlg->m_LayerMgr.GetLayer("RIP"))->TriggeredUpdate(update,"다보내라-_-");													
		
		g_cs2.Unlock();
		Sleep(5000);
	}
	return 0;
}

BOOL CDynamicRouterDlg::Send( unsigned char* ppayload, int nlength,unsigned char *dstAddr, unsigned char *deviceDescriptor )
{
	BOOL bSuccess=FALSE;
	CIpLayer* ipLayer=(CIpLayer*)(FindCompatibleLayer(2,(char*)deviceDescriptor));			

	if(ipLayer->Send(ppayload,nlength,dstAddr,FALSE))
		bSuccess=TRUE;

	return bSuccess;
}

void CDynamicRouterDlg::OnButtonRoutingAllclear() 
{
	BOOL update[MAX_ROUTING_TABLE];
	int i=0;
		
	for(i=0;i<MAX_ROUTING_TABLE;i++)
		update[i]=FALSE;

	for(i=0;i<MAX_ROUTING_TABLE;i++){
		if(m_routingTable[i]){
			update[i]=TRUE;
			m_routingTable[i]->metric=16;
		}	
	}

	// start버튼이 눌려서 쓰레드가 돌고 있을경우 지워진 정보를 triggered
	if(m_bSetButton)							
		((CRIP*)mp_UnderLayer)->TriggeredUpdate(update,"다보내라~~-_-");
	
	for(i=0;i<MAX_ROUTING_TABLE;i++){
		if(m_routingTable[i]){
			free(m_routingTable[i]);
			m_routingTable[i]=NULL;
		}	
	}

	m_listctrlRouting.DeleteAllItems();	
}

void CDynamicRouterDlg::OnItemchangedListRoutingTable(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_nSelectedItem=pNMListView->iItem;
	*pResult = 0;
}

void CDynamicRouterDlg::OnButtonSave() 
{
	FILE *fp=fopen("StaticTable.txt","w");
	if(!fp){
		AfxMessageBox("file save error");
		return;
	}
	for(int i=0;i<MAX_ROUTING_TABLE;i++){
		if(m_routingTable[i]){			
			fwrite(m_routingTable[i],sizeof(CRoutingTable::ROUTING_TABLE),1,fp);	
		}
	}
	fclose(fp);	
}

void CDynamicRouterDlg::OnButtonOpen() 
{
	char buff[232]={0x00}, charMetric[5]={0x00};
	int availableNum=0,nCount=0, i=0;
	LV_ITEM lvItem;	
	CRoutingTable::LPROUTING_TABLE temp;
		
	FILE *fp=fopen("StaticTable.txt","r");
	
	if(!fp){		
		AfxMessageBox("file save error");
		return;
	}

	for(i=0;i<MAX_ROUTING_TABLE;i++){
		if(m_routingTable[i]){
			free(m_routingTable[i]);
			m_routingTable[i]=NULL;
		}
	}
	RefreshRoutingTable();
	
	while(fread(buff,sizeof(CRoutingTable::_ROUTING_TABLE),1,fp)!=0){				
		temp=(CRoutingTable::LPROUTING_TABLE)buff;
		availableNum=FindAvailableStaticTableNum();		
		m_routingTable[availableNum]=(CRoutingTable::LPROUTING_TABLE)malloc(sizeof(CRoutingTable::ROUTING_TABLE));
		memcpy(m_routingTable[availableNum]->destination.S_ip_addr,temp->destination.S_ip_addr,4);
		memcpy(m_routingTable[availableNum]->netmask.S_ip_addr,temp->netmask.S_ip_addr,4);
		memcpy(m_routingTable[availableNum]->gateway.S_ip_addr,temp->gateway.S_ip_addr,4);
		sprintf(m_routingTable[availableNum]->flag,"%s",temp->flag);
		sprintf(m_routingTable[availableNum]->device_descripter,"%s",temp->device_descripter);
		m_routingTable[availableNum]->metric=temp->metric;
		m_routingTable[availableNum]->cnt=0;
	}		
	
	nCount=m_listctrlRouting.GetItemCount();
	
	for(i=MAX_ROUTING_TABLE-1;i>=0;i--){		//출력순서를 맞추기위해서 배열의 끝에서부터 출력
		if(m_routingTable[i]){
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=0;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->destination.S_ip_addr);
			m_listctrlRouting.InsertItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=1;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->netmask.S_ip_addr);;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=2;	
			lvItem.pszText=IpAddrNtoS(m_routingTable[i]->gateway.S_ip_addr);;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=3;	
			lvItem.pszText=m_routingTable[i]->flag;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=4;	
			lvItem.pszText=m_routingTable[i]->device_descripter;
			m_listctrlRouting.SetItem(&lvItem);
			
			lvItem.mask=LVIF_TEXT;
			lvItem.iItem=nCount;
			lvItem.iSubItem=5;
			sprintf(charMetric,"%d",m_routingTable[i]->metric);
			lvItem.pszText=charMetric;
			m_listctrlRouting.SetItem(&lvItem);
		}
	}
	fclose(fp);
	
}

// 사용가능한 라우팅 테이블 행을 알려 줌
int CDynamicRouterDlg::FindAvailableStaticTableNum()
{ 
		
	for( int i=0 ; i<MAX_ROUTING_TABLE ; i++ ){
		if( !m_routingTable[i] )
			return i;		//첫번째 NULL포인터의 위치		
	}
	return -1;	
}

// 현재 프로토콜 스택에서 원하는 레이어를 리턴해 줌
CBaseLayer* CDynamicRouterDlg::FindCompatibleLayer(int layerNum,char *NICDescription)
{
	CBaseLayer *layer=NULL;
	CString str1,str2;
	char *ch;

	for(int i=1; i <= m_nadapterNum ; i++)
	{	
		switch(layerNum){
			case 1:
				ch=(char*)malloc(sizeof(char)*20);
				sprintf(ch,"UDP%d",i);
				layer=m_LayerMgr.GetLayer(ch);
				str1.Format("%s",((CUDP*)layer)->m_NICDescription);
				break;
			case 2:
				ch=(char*)malloc(sizeof(char)*20);
				sprintf(ch,"IP%d",i);
				layer=m_LayerMgr.GetLayer(ch);
				str1.Format("%s",((CIpLayer*)layer)->m_NICDescription);
				break;
			case 3:
				ch=(char*)malloc(sizeof(char)*20);
				sprintf(ch,"Arp%d",i);
				layer=m_LayerMgr.GetLayer(ch);				
				str1.Format("%s",((CArpLayer*)layer)->m_NICDescription);				
				break;
		}
		
		str2.Format("%s",NICDescription);
		if(!str1.Compare(str2))	{					
			return layer;
		}
	}	// end of for(int i=1; i <= m_nadapterNum ; i++)
	return NULL;
}


