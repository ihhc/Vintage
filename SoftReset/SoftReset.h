// SoftReset.h : main header file for the SOFTRESET application
//

#if !defined(AFX_SOFTRESET_H__FB0F2BA2_F348_48A3_B0D7_9DC2A01CA430__INCLUDED_)
#define AFX_SOFTRESET_H__FB0F2BA2_F348_48A3_B0D7_9DC2A01CA430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSoftResetApp:
// See SoftReset.cpp for the implementation of this class
//

class CSoftResetApp : public CWinApp
{
public:
	CSoftResetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftResetApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSoftResetApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTRESET_H__FB0F2BA2_F348_48A3_B0D7_9DC2A01CA430__INCLUDED_)
