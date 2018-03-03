// LinkHelper.h : main header file for the LINKHELPER application
//

#if !defined(AFX_LINKHELPER_H__D5953CB5_BC55_4854_9F86_F3A927BA7C0A__INCLUDED_)
#define AFX_LINKHELPER_H__D5953CB5_BC55_4854_9F86_F3A927BA7C0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLinkHelperApp:
// See LinkHelper.cpp for the implementation of this class
//

class CLinkHelperApp : public CWinApp
{
public:
	CLinkHelperApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkHelperApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLinkHelperApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKHELPER_H__D5953CB5_BC55_4854_9F86_F3A927BA7C0A__INCLUDED_)
