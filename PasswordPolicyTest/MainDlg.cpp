// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include ".\maindlg.h"
#include "Lm.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	return TRUE;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHAR szPsw[MAX_PATH];
	::GetWindowText(GetDlgItem(IDC_EDIT1), szPsw, MAX_PATH);

	NET_VALIDATE_OUTPUT_ARG Output = {0};
	NET_VALIDATE_AUTHENTICATION_INPUT_ARG Input = {0};
	Input.PasswordMatched = TRUE;

	NET_VALIDATE_PASSWORD_HASH PasswordHistory;
	
	size_t lLength = strlen(szPsw);
	PasswordHistory.Length = lLength;
	PasswordHistory.Hash = new BYTE[lLength];
	memcpy(PasswordHistory.Hash, szPsw, lLength);
	Input.InputPersistedFields.PasswordHistory = &PasswordHistory;

	DWORD dwErr = NetValidatePasswordPolicy(NULL, NULL, NetValidateAuthentication, &Input, (void**)&Output);
	
	if (NERR_Success == dwErr)
	{
		if (Output.ValidationStatus != NERR_Success)
			MessageBox("The password has not been authenticated", _T("NetValidatePasswordPolicy"));
		else
			MessageBox("The password is authenticated", _T("NetValidatePasswordPolicy"));
	}
	else
	{
		LPVOID lpMsgBuf = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
		MessageBox((LPCSTR)lpMsgBuf, _T("NetValidatePasswordPolicy"));
		LocalFree(lpMsgBuf);
	}

	NetValidatePasswordPolicyFree((void**)&Output);
	return 0;
}
