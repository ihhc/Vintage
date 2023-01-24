// SoftResetDlg.h : header file
//

#if !defined(AFX_SOFTRESETDLG_H__57AEB4DC_A48F_4918_A482_19B35465A373__INCLUDED_)
#define AFX_SOFTRESETDLG_H__57AEB4DC_A48F_4918_A482_19B35465A373__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSoftResetDlg dialog

class CSoftResetDlg : public CDialog
{
// Construction
public:
	CSoftResetDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSoftResetDlg)
	enum { IDD = IDD_SOFTRESET_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftResetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSoftResetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTRESETDLG_H__57AEB4DC_A48F_4918_A482_19B35465A373__INCLUDED_)
