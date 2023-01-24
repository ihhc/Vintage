// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include <mstask.h>
#include <atltypes.h>
#include <atlstr.h>
#include <comdef.h>

#pragma comment(lib, "mstask.lib")

#define DDS_CST_EVERYMONTH \
                        TASK_JANUARY  | TASK_FEBRUARY | TASK_MARCH     | \
                        TASK_APRIL    | TASK_MAY      | TASK_JUNE      | \
                        TASK_JULY     | TASK_AUGUST   | TASK_SEPTEMBER | \
                        TASK_OCTOBER  | TASK_NOVEMBER | TASK_DECEMBER  

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

	m_EnableTask = GetDlgItem(IDC_ENABLETASK);
	m_Num = GetDlgItem(IDC_NUM);
	m_Type = GetDlgItem(IDC_TYPE);
	m_StartTime = GetDlgItem(IDC_STARTTIME);
	m_StartDate = GetDlgItem(IDC_STARTDATE);
	m_Task = GetDlgItem(IDC_TASK);
	m_Command = GetDlgItem(IDC_COMMAND);
	m_Params = GetDlgItem(IDC_PARAMS);
	
	m_Type.AddString("Hour");
	m_Type.AddString("Day");
	m_Type.AddString("Week");
	m_Type.AddString("Month");
	m_Type.SetCurSel(1);

	SYSTEMTIME st;
	GetLocalTime(&st);
	m_StartTime.SetSystemTime(0, &st);
	m_StartDate.SetSystemTime(0, &st);

	return TRUE;
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

LRESULT CMainDlg::OnBnClickedButton3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE, _T("*"), NULL, OFN_FILEMUSTEXIST, _T("All Files (*.*)\0*.*\0"), m_hWnd);
    dlg.m_ofn.lpstrTitle = "Select file";
    if (dlg.DoModal() == IDOK)
		m_Command.SetWindowText(dlg.m_szFileName);
	return 0;
}

LRESULT CMainDlg::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Load();
	return 0;
}

LRESULT CMainDlg::OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Save();
	return 0;
}

void CMainDlg::Save()
{
	CString sTask;
	m_Task.GetWindowText(sTask);
	CComPtr<ITaskScheduler> pISched  = NULL;
	try
	{
		::CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (void**)&pISched );
		if (m_EnableTask.GetCheck()!=BST_CHECKED)
		{
			pISched->Delete(_bstr_t(sTask));
			return;
		}
		CComPtr<IEnumWorkItems> pItems;
		pISched->Enum(&pItems);
//------------------------------------------------
		LPWSTR *lpwszNames;
		DWORD dwFetchedTasks = 0;
		BOOL bFound = FALSE;
		BOOL bDelete = FALSE;
		while (SUCCEEDED(pItems->Next(1, &lpwszNames, &dwFetchedTasks)) && (dwFetchedTasks != 0))
		{
			while (dwFetchedTasks)
			{
				CString s = lpwszNames[--dwFetchedTasks];
				s = s.Left(s.ReverseFind('.'));
				CoTaskMemFree(lpwszNames[dwFetchedTasks]);
				if (s == sTask)
					bFound =TRUE;
			}
			CoTaskMemFree(lpwszNames);
		}
		SYSTEMTIME st1, stStartDate;
		m_StartTime.GetSystemTime(&st1);
		m_StartDate.GetSystemTime(&stStartDate);
		stStartDate.wHour = st1.wHour;
		stStartDate.wMinute = st1.wMinute;
		stStartDate.wSecond = st1.wSecond;
		stStartDate.wMilliseconds = st1.wMilliseconds;
		int icNum = GetDlgItemInt(IDC_NUM);
		if (icNum<=0)
			icNum = 1;
		int icType = m_Type.GetCurSel();
		if (bFound)
		{
			int iNum, iType;
			CComPtr<IScheduledWorkItem> pITask;
			pISched->Activate(_bstr_t(sTask), IID_ITask, (IUnknown**) &pITask);
			HRESULT hst;
			pITask->GetStatus(&hst);
			if ((SCHED_S_TASK_READY != hst) && (SCHED_S_TASK_HAS_NOT_RUN != hst))
				bDelete = TRUE;
			else
			{
				SYSTEMTIME stBegin;
				DOUBLE d = 0; VariantTimeToSystemTime(d, &stBegin);
				LPSYSTEMTIME pstTaskTimes;
				WORD wCountOfRuns = 2;
				pITask->GetRunTimes(&stBegin, NULL, &wCountOfRuns, &pstTaskTimes);
				if (wCountOfRuns==0)
					bDelete = TRUE;
				else
				{
					DOUBLE dStartTime1, dStartTime2;
					SystemTimeToVariantTime(&stStartDate, &dStartTime1);
					SystemTimeToVariantTime(pstTaskTimes, &dStartTime2);
					if (dStartTime1!=dStartTime2)
						bDelete = TRUE;
					else
					{
						if (wCountOfRuns==1)
						{
							iNum = 1;
							iType = 1;
						}
						else
						{
							DOUBLE d1;
							SystemTimeToVariantTime(pstTaskTimes, &d);
							pstTaskTimes++;
							SystemTimeToVariantTime(pstTaskTimes, &d1);
							DOUBLE dd = d1-d;
							if ((int)dd>=28)
							{
								iNum = 1;
								iType = 3;
							}
							else if ((int)dd>7)
							{
								iNum = (int)dd;
								iType = 3;
							}
							else if ((int)dd==7)
							{
								iNum = 1;
								iType = 2;
							}
							else if ((((int)dd)<7)&&(((int)dd)>1))
							{
								iNum = (int)dd;
								iType = 2;
							}
							else if (((int)dd)==1)
							{
								iNum = 1;
								iType = 1;
							}
							else if (dd >= 0.0417) 
							{
								iNum = (int)(dd/0.0416);
								iType = 0;
							}
							else 
							{
								iNum = 1;
								iType = 0;
							}
						}
						if ((icType!= iType)||(icNum!=iNum))
							bDelete = TRUE;
					}
				}
			}
			if (bDelete)
				pISched->Delete(_bstr_t(sTask));
		}
		if ((!bFound)||(bDelete))
		{
			CComPtr<ITask> pITask;
			pISched->NewWorkItem(_bstr_t(sTask), CLSID_CTask, IID_ITask, (IUnknown**) &pITask);
			//pITask->SetWorkingDirectory(_bstr_t(sqhfile));
			CString sCommand;
			m_Command.GetWindowText(sCommand);
			pITask->SetApplicationName(_bstr_t(sCommand));
			CString sparam;
			m_Params.GetWindowText(sparam);
			if (sparam.GetLength()>0)
				pITask->SetParameters(_bstr_t(sparam));
			DWORD dwTaskFlags = TASK_FLAG_DONT_START_IF_ON_BATTERIES | TASK_FLAG_KILL_IF_GOING_ON_BATTERIES;
			pITask->SetFlags(dwTaskFlags);
			pITask->SetAccountInformation( L"", NULL );
			CComPtr<ITaskTrigger> pITaskTrig = NULL;
			TASK_TRIGGER rTrigger;
			WORD wTrigNumber;
			if (icType!=0)
			{
				pITask->CreateTrigger(&wTrigNumber, &pITaskTrig);
				ZeroMemory(&rTrigger, sizeof (TASK_TRIGGER));
				rTrigger.cbTriggerSize = sizeof (TASK_TRIGGER);
				rTrigger.wBeginYear    = stStartDate.wYear;
				rTrigger.wBeginMonth   = stStartDate.wMonth;
				rTrigger.wBeginDay     = stStartDate.wDay;
				rTrigger.wStartHour    = stStartDate.wHour;
				rTrigger.wStartMinute  = stStartDate.wMinute;

				if (icType == 3)
				{
					rTrigger.TriggerType = TASK_TIME_TRIGGER_MONTHLYDATE;
					rTrigger.Type.MonthlyDate.rgfDays = 1 << ( stStartDate.wDay - 1 );
					rTrigger.Type.MonthlyDate.rgfMonths = DDS_CST_EVERYMONTH;
				}
				if (icType == 2)
				{
					static WORD s_wDayFlags[] = { 0, TASK_MONDAY, TASK_TUESDAY,
                            TASK_WEDNESDAY, TASK_THURSDAY, TASK_FRIDAY, TASK_SATURDAY, TASK_SUNDAY };
					rTrigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;
					rTrigger.Type.Weekly.rgfDaysOfTheWeek =  s_wDayFlags[stStartDate.wDayOfWeek]; 
					rTrigger.Type.Weekly.WeeksInterval = icNum;
				}
				if (icType == 1)
				{
					rTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;
					rTrigger.Type.Daily.DaysInterval = icNum;
				}
				pITaskTrig->SetTrigger(&rTrigger);
			}
			else
			{
				int iHour = 1;
				while ( iHour < 24 )
				{
					pITask->CreateTrigger(&wTrigNumber, &pITaskTrig);
					ZeroMemory(&rTrigger, sizeof (TASK_TRIGGER));
					rTrigger.cbTriggerSize = sizeof (TASK_TRIGGER);
					rTrigger.wBeginYear    = stStartDate.wYear;
					rTrigger.wBeginMonth   = stStartDate.wMonth;
					rTrigger.wBeginDay     = stStartDate.wDay;
					rTrigger.wStartHour    = iHour;
					rTrigger.wStartMinute  = stStartDate.wMinute;
					rTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;
					rTrigger.Type.Daily.DaysInterval = 1;
					
					pITaskTrig->SetTrigger(&rTrigger);

					iHour += icNum;
					pITaskTrig = NULL;
				}
			}
			CComPtr<IPersistFile>  pIFile = NULL;
			pITask->QueryInterface(IID_IPersistFile, (void**)&pIFile);
			pIFile->Save(NULL, FALSE);
		}
//------------------------------------------------
	}
	catch(_com_error &e)
	{
		CString msg = "Task scheduler interface error.\n";
		msg += (LPCSTR)e.Description(); msg += "\n";
		msg += (LPCSTR)e.Error();
		MessageBox(msg, e.Source());
	}
}

void CMainDlg::Load()
{
	CString sTask;
	m_Task.GetWindowText(sTask);
	CComPtr<ITaskScheduler> pISched  = NULL;
    try
	{
		::CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (void**)&pISched );
		CComPtr<IEnumWorkItems> pItems;
		pISched->Enum(&pItems);

		LPWSTR *lpwszNames;
		DWORD dwFetchedTasks = 0;
		BOOL bFound = FALSE;
		while (SUCCEEDED(pItems->Next(1, &lpwszNames, &dwFetchedTasks)) && (dwFetchedTasks != 0)&&(!bFound))
		{
			while (dwFetchedTasks)
			{
				CString s = lpwszNames[--dwFetchedTasks];
				s = s.Left(s.ReverseFind('.'));
				CoTaskMemFree(lpwszNames[dwFetchedTasks]);
				if (s == sTask)
					bFound =TRUE;
			}
			CoTaskMemFree(lpwszNames);
		}
		if (!bFound)
		{
			MessageBox("Task not found", "Load task", MB_OK);
			return;
		}
		CComPtr<ITask> pITask;
		pISched->Activate(_bstr_t(sTask), IID_ITask, (IUnknown**) &pITask);
		HRESULT hst;
		
		LPWSTR lpwszApplicationName;
		pITask->GetApplicationName(&lpwszApplicationName);
		CString sName = lpwszApplicationName;
		m_Command.SetWindowText(sName);
		CoTaskMemFree(lpwszApplicationName);

		LPWSTR lpwszParameters;
		pITask->GetParameters(&lpwszParameters);
		CString sParams = lpwszParameters;
		m_Params.SetWindowText(sParams);
		CoTaskMemFree(lpwszParameters);
		
		pITask->GetStatus(&hst);
		if ((SCHED_S_TASK_RUNNING != hst) && (SCHED_S_TASK_READY != hst) && (SCHED_S_TASK_HAS_NOT_RUN != hst))
			m_EnableTask.SetCheck(BST_UNCHECKED);
		else
			m_EnableTask.SetCheck(BST_CHECKED);
		SYSTEMTIME stBegin;
		DOUBLE d = 0; VariantTimeToSystemTime(d, &stBegin);
		LPSYSTEMTIME pstTaskTimes;
		WORD wCountOfRuns = 2;
		pITask->GetRunTimes(&stBegin, NULL, &wCountOfRuns, &pstTaskTimes);
		if (wCountOfRuns==2)
		{
			m_StartTime.SetSystemTime(0, pstTaskTimes);
			m_StartDate.SetSystemTime(0, pstTaskTimes);

			DOUBLE d1;
			SystemTimeToVariantTime(pstTaskTimes, &d);
			pstTaskTimes++;
			SystemTimeToVariantTime(pstTaskTimes, &d1);
			DOUBLE dd = d1-d;
			if ((int)dd>=30)
			{
				m_Num.SetWindowText("1");
				m_Type.SetCurSel(3);
			}
			else if ((int)dd>7)
			{
				char ch[4]; ch[3] = '\0';
				itoa((int)(dd/7), ch, 10);
				m_Num.SetWindowText(ch);
				m_Type.SetCurSel(2);
			}
			else if ((int)dd==7)
			{
				m_Num.SetWindowText("1");
				m_Type.SetCurSel(2);
			}
			else if ((((int)dd)<7)&&(((int)dd)>1))
			{
				char ch[4]; ch[3] = '\0';
				itoa((int)dd, ch, 10);
				m_Num.SetWindowText(ch);
				m_Type.SetCurSel(1);
			}
			else if (((int)dd)==1)
			{
				m_Num.SetWindowText("1");
				m_Type.SetCurSel(1);
			}
			else if (dd >= 0.0416) 
			{
				char ch[4]; ch[3] = '\0';
				DOUBLE id = dd/0.0416; 
				itoa((int)id, ch, 10);
				m_Num.SetWindowText(ch);
				m_Type.SetCurSel(0);
			}
			else
			{
				m_Num.SetWindowText("1");
				m_Type.SetCurSel(0);
			}
		}
	} 
	catch(_com_error &e)
	{
		CString msg = "Task scheduler interface error.\n";
		msg += (LPCSTR)e.Description(); msg += "\n";
		msg += (LPCSTR)e.Error();
		MessageBox(msg, e.Source());
	}
}

void CMainDlg::SetText()
{
	int is = 0, ie = 0;
	m_Num.GetSel(is, ie);
	int iNum = GetDlgItemInt(IDC_NUM);
	if (iNum < 1)
	{
		SetDlgItemInt(IDC_NUM, 1);
		m_Num.SetSel(is, ie);
		return;
	}
	int iType = m_Type.GetCurSel();
	if ((iType == 3)&&(iNum>1))
	{
		SetDlgItemInt(IDC_NUM, 1);
		m_Num.SetSel(is, ie);
		return;
	}
	if ((iType == 2)&&(iNum>7))
	{
		SetDlgItemInt(IDC_NUM, 7);
		m_Num.SetSel(is, ie);
		return;
	}
	if ((iType == 1)&&(iNum>30))
	{
		SetDlgItemInt(IDC_NUM, 30);
		m_Num.SetSel(is, ie);
		return;
	}
	if ((iType == 0)&&(iNum>24))
	{
		SetDlgItemInt(IDC_NUM, 24);
		m_Num.SetSel(is, ie);
		return;
	}
}

LRESULT CMainDlg::OnEnChangeNum(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	return 0;
}

LRESULT CMainDlg::OnCbnSelendcancelType(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}
