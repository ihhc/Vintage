// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include ".\maindlg.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

BOOL CMainDlg::RefreshConnection()
{
	CString sCon ="PROVIDER=SQLOLEDB.1;SERVER=ib;DATABASE=northwind;UID=test;PWD=;";;
	try
	{
		if (m_pCon == NULL)
			CoCreateInstance(__uuidof(ADODB::Connection),NULL, CLSCTX_ALL, __uuidof(ADODB::_Connection), (void**)&m_pCon);
		if (m_pCon==NULL)
		{
			::MessageBox(NULL, _T("ADO Connection object creation failed"), _T("Connection error"), MB_OK);
			return FALSE;
		}
		if (m_pCon->State == adStateOpen) return TRUE;
		m_pCon->CursorLocation = adUseClient;
		m_pCon->Open(_bstr_t((LPCTSTR)sCon), L"", L"", adConnectUnspecified);
		if( (m_pCon->Errors->Count) > 0)
		{
			long nCount = m_pCon->Errors->Count;
			for(long i = 0; i < nCount; i++)
				::MessageBox(NULL, (LPCTSTR)m_pCon->Errors->GetItem(i)->Description, _T("Connection error"), MB_OK);
			return FALSE;
		}

	}
	catch( _com_error &e)
	{
		::MessageBox(NULL,(LPCTSTR)e.Description(),(LPCTSTR)e.Source(), MB_OK);
		m_pCon.Release();
		return FALSE;
	}
	return TRUE;
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


	::SetWindowText(GetDlgItem(IDC_EDIT1), "C:\\temp\\test.htm");

	RefreshConnection();
	return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
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
	char sf[MAX_PATH];
	::GetWindowText(GetDlgItem(IDC_EDIT1), sf, MAX_PATH);
	_RecordsetPtr  pRec;
	try
	{
		CoCreateInstance(__uuidof(Recordset), NULL, CLSCTX_ALL, __uuidof(_Recordset), (void**)&pRec);
		pRec->CursorLocation = adUseClient;
		pRec->Open(_bstr_t("select content from testtable"), _variant_t((IDispatch *)m_pCon,true), adOpenForwardOnly, adLockOptimistic, adCmdUnspecified);
		pRec->AddNew();
		
		HANDLE hfile = CreateFile(sf,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 
		BYTE* byteToSaveA = NULL;
		DWORD dwSizeToSave = ::GetFileSize (hfile, NULL) ; 
		byteToSaveA = new BYTE[dwSizeToSave+1];
		DWORD dwrSize = 0;
		ReadFile(hfile, byteToSaveA, dwSizeToSave, &dwrSize, NULL);
		CloseHandle(hfile);

		SAFEARRAY* psa; 
		psa = SafeArrayCreateVector(VT_UI1, 0, dwSizeToSave); 
		void* pTemp = NULL;
		SafeArrayAccessData(psa, (void HUGEP**)&pTemp);
		memcpy( pTemp, byteToSaveA, dwSizeToSave);
		SafeArrayUnaccessData(psa);   

		delete [] byteToSaveA;

		VARIANT vData;                                         
		vData.vt = VT_ARRAY | VT_UI1;
		vData.parray = psa;
		pRec->Fields->Item[L"content"]->AppendChunk(vData);

		pRec->Update();
		pRec->Close();

		pRec->Open(_bstr_t("select content from testtable"), _variant_t((IDispatch *)m_pCon,true), adOpenForwardOnly, adLockOptimistic, adCmdUnspecified);
		VARIANT vtest;
		pRec->Fields->Item[L"content"]->get_Value(&vtest);
		SAFEARRAY* psa1 = vtest.parray; 
		BYTE HUGEP * pbstr;
		SafeArrayAccessData(psa1, (void HUGEP**)&pbstr);
		SafeArrayUnaccessData(psa1);
		::SetWindowText(GetDlgItem(IDC_EDIT2), (LPCSTR)pbstr);
		pRec->Delete(adAffectCurrent);
	}
	catch( _com_error &e)
	{
		CString msg = (LPCSTR)e.Description();
		msg += ". ";
		msg += (LPCSTR)e.Source();
		MessageBox(msg , e.ErrorMessage() , MB_OK);
	}

	return 0;
}
