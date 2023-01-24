// SoftResetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoftReset.h"
#include "SoftResetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <winioctl.h>

#define IOCTL_HAL_REBOOT CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

extern "C" __declspec(dllimport) BOOL KernelIoControl(
	DWORD dwIoControlCode, 
	LPVOID lpInBuf, 
	DWORD nInBufSize, 
	LPVOID lpOutBuf, 
	DWORD nOutBufSize, 
	LPDWORD lpBytesReturned);

/////////////////////////////////////////////////////////////////////////////
// CSoftResetDlg dialog

CSoftResetDlg::CSoftResetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftResetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoftResetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSoftResetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoftResetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSoftResetDlg, CDialog)
	//{{AFX_MSG_MAP(CSoftResetDlg)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftResetDlg message handlers

BOOL CSoftResetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CSoftResetDlg::OnCancel() 
{
	EndDialog(IDCANCEL);
}

void CSoftResetDlg::OnReset() 
{
	KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
}
