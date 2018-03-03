// LinkHelperDlg.h : header file
//

#if !defined(AFX_LINKHELPERDLG_H__F551E536_4C2F_45A6_93E9_A686EDA31E7D__INCLUDED_)
#define AFX_LINKHELPERDLG_H__F551E536_4C2F_45A6_93E9_A686EDA31E7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CLinkHelperDlg dialog
#include "DragEdit.h"

class CLinkHelperDlg : public CDialog
{
// Construction
public:
	CLinkHelperDlg(CWnd* pParent = NULL);	// standard constructor
	CToolTipCtrl* m_tipBtnCtr;
// Dialog Data
	//{{AFX_DATA(CLinkHelperDlg)
	enum { IDD = IDD_LINKHELPER_DIALOG };
	CDragEdit	m_DragDst;
	CDragEdit	m_DragEdit;
	CString	m_szDst;
	CString	m_szSrc;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkHelperDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLinkHelperDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKHELPERDLG_H__F551E536_4C2F_45A6_93E9_A686EDA31E7D__INCLUDED_)
