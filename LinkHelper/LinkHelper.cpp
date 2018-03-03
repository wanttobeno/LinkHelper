// LinkHelper.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LinkHelper.h"
#include "LinkHelperDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkHelperApp

BEGIN_MESSAGE_MAP(CLinkHelperApp, CWinApp)
	//{{AFX_MSG_MAP(CLinkHelperApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkHelperApp construction

CLinkHelperApp::CLinkHelperApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLinkHelperApp object

CLinkHelperApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLinkHelperApp initialization

BOOL CLinkHelperApp::InitInstance()
{
	// Standard initialization

	CLinkHelperDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
