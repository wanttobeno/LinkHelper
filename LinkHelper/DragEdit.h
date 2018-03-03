/*
	支持文件拖入，纸质ToolTip
*/
#if !defined(AFX_DRAGEDIT_H__951CC563_3B8D_4F24_8824_0E417FFF1B32__INCLUDED_)
#define AFX_DRAGEDIT_H__951CC563_3B8D_4F24_8824_0E417FFF1B32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragEdit window

class CDragEdit : public CEdit
{
// Construction
public:
	CDragEdit();

// Attributes
public:

// Operations
public:
	CToolTipCtrl* m_tipBtnCtr;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragEdit)
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGEDIT_H__951CC563_3B8D_4F24_8824_0E417FFF1B32__INCLUDED_)
