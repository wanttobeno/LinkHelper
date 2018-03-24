// LinkHelperDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LinkHelper.h"
#include "LinkHelperDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MLink.h"

/////////////////////////////////////////////////////////////////////////////
// CLinkHelperDlg dialog

CLinkHelperDlg::CLinkHelperDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinkHelperDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkHelperDlg)
	m_szDst = _T("");
	m_szSrc = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLinkHelperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkHelperDlg)
	DDX_Control(pDX, IDC_DST, m_DragDst);
	DDX_Control(pDX, IDC_SRC, m_DragEdit);
	DDX_Text(pDX, IDC_DST, m_szDst);
	DDX_Text(pDX, IDC_SRC, m_szSrc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkHelperDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkHelperDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkHelperDlg message handlers

BOOL CLinkHelperDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//解除window7以上窗口对该消息的过滤
	typedef BOOL(__stdcall * MsMessageFilter)(UINT message, DWORD dwFlag);
	HMODULE hMod = LoadLibrary(_T("user32.dll"));
	if (hMod)
	{
		MsMessageFilter  MsChangeWinMessageFilter;
		MsChangeWinMessageFilter = (MsMessageFilter)GetProcAddress(hMod, "ChangeWindowMessageFilter");
		if (MsChangeWinMessageFilter)
		{
			MsChangeWinMessageFilter(WM_DROPFILES, 1);
			MsChangeWinMessageFilter(0x0049, 1);// 0x0049 == WM_COPYGLOBALDATA
		}
		FreeLibrary(hMod);
	}
	

	// 中间置顶显示
	int cxScreen,cyScreen;

	cxScreen=GetSystemMetrics(SM_CXSCREEN);
	cyScreen=GetSystemMetrics(SM_CYSCREEN);
	
	RECT rt;
	GetWindowRect(&rt);
	int nX = (cxScreen - rt.right)/2;
	int nY = (cyScreen - rt.bottom)/2;
	SetWindowPos(&wndTopMost,nX,nY,rt.right,rt.bottom,SWP_SHOWWINDOW);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLinkHelperDlg::OnPaint() 
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

HCURSOR CLinkHelperDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#include <stdio.h>
void CLinkHelperDlg::OnOK() 
{
	//FILE *pFile = fopen("C:\\Users\\ZhanYue\\Desktop\\LinkHelper\\123DragEdit.cpp ","rb");
	//fseek(pFile,0, SEEK_END);
	//long size=ftell (pFile);
	//fclose(pFile);

	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_szDst.IsEmpty()||m_szSrc.IsEmpty())
		return;
	CMLink  link;
	bool bRet = false;
	TCHAR* pSave = m_szDst.GetBuffer(m_szDst.GetLength());
	TCHAR* pDst = m_szSrc.GetBuffer(m_szSrc.GetLength());
	if (m_szDst.CompareNoCase(pDst)==0)
	{
		::MessageBox(m_hWnd,_T("目标文件路径相同！"),_T("提示"),MB_OK);
		return;
	}
	DWORD dwAttr =  GetFileAttributes(pDst);
	if (dwAttr  != -1)
	{
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			bRet = link.MakeLink(pSave,pDst,SOFTLINKD);
		}
		else
		{
			bRet = link.MakeLink(pSave,pDst,SOFTLINKF);
		}	
}
	else
	{
		::MessageBox(m_hWnd,_T("目标文件不存在！"),_T("提示"),MB_OK);
	}
	if(bRet)
		::MessageBox(m_hWnd,_T("链接成功!"),_T("提示"),MB_OK);
	int n=0;

	//CDialog::OnOK();
}

BOOL CLinkHelperDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
// 	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP)
// 	{
// 		m_tipBtnCtr->RelayEvent(pMsg); 
// 	}
	return CDialog::PreTranslateMessage(pMsg);
}
