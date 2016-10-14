; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDynamicRouterDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DynamicRouter.h"

ClassCount=5
Class1=CDynamicRouterApp
Class2=CDynamicRouterDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_DIALOG_ADD_ROUTE
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=CAddStaticTable
Resource4=IDD_DynamicRouter_DIALOG
Class5=CProxyARP
Resource5=IDD_DIALOG_PROXY_ARP

[CLS:CDynamicRouterApp]
Type=0
HeaderFile=DynamicRouter.h
ImplementationFile=DynamicRouter.cpp
Filter=N

[CLS:CDynamicRouterDlg]
Type=0
HeaderFile=DynamicRouterDlg.h
ImplementationFile=DynamicRouterDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CDynamicRouterDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=DynamicRouterDlg.h
ImplementationFile=DynamicRouterDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DynamicRouter_DIALOG]
Type=1
Class=CDynamicRouterDlg
ControlCount=24
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_LIST_ROUTING_TABLE,SysListView32,1350664197
Control4=IDC_LIST_CACHE_TABLE,SysListView32,1350664197
Control5=IDC_BUTTON_ROUTING_ADD,button,1342242816
Control6=IDC_BUTTON_ROUTING_ALLCLEAR,button,1342242816
Control7=IDC_BUTTON_CACHE_DELETE,button,1342242816
Control8=IDC_STATIC,button,1342177287
Control9=IDC_COMBO_NIC,combobox,1344340226
Control10=IDC_EDIT_IPADDR,edit,1350631552
Control11=IDC_EDIT_MACADDR,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_BUTTON_START,button,1342242816
Control16=IDC_STATIC,button,1342177287
Control17=IDC_LIST_PROXY,SysListView32,1350664197
Control18=IDC_BUTTON_PROXY_ADD,button,1342242816
Control19=IDC_BUTTON_PROXY_DELETE,button,1342242816
Control20=IDC_BUTTON_SET,button,1342242816
Control21=IDC_BUTTON_END,button,1342242816
Control22=IDC_BUTTON_ROUTING_DELETE,button,1342242816
Control23=IDC_BUTTON_SAVE,button,1342242816
Control24=IDC_BUTTON_OPEN,button,1342242816

[DLG:IDD_DIALOG_ADD_ROUTE]
Type=1
Class=CAddStaticTable
ControlCount=17
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUTTON_ADD,button,1342242816
Control8=IDC_BUTTON_CANCEL,button,1342242816
Control9=IDC_EDIT_DESTINATION,edit,1350631552
Control10=IDC_EDIT_GATEWAY,edit,1350631552
Control11=IDC_EDIT_NETMASK,edit,1350631552
Control12=IDC_CHECK_UP,button,1342242819
Control13=IDC_CHECK_GATEWAY,button,1342242819
Control14=IDC_CHECK_HOST,button,1342242819
Control15=IDC_COMBO_INTERFACE,combobox,1344340226
Control16=IDC_STATIC,static,1342308352
Control17=IDC_EDIT_METRIC,edit,1350631552

[CLS:CAddStaticTable]
Type=0
HeaderFile=AddStaticTable.h
ImplementationFile=AddStaticTable.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_CHECK_HOST

[DLG:IDD_DIALOG_PROXY_ARP]
Type=1
Class=CProxyARP
ControlCount=7
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COMBO_DEVICE,combobox,1344340226
Control5=IDC_EDIT_IPADDRESS,edit,1350631552
Control6=IDC_BUTTON_OK,button,1342242816
Control7=IDC_BUTTON_CANCEL,button,1342242816

[CLS:CProxyARP]
Type=0
HeaderFile=proxyarp.h
ImplementationFile=proxyarp.cpp
BaseClass=CDialog
LastObject=CProxyARP
Filter=D
VirtualFilter=dWC

