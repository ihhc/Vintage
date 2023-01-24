// WaitProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tlhelp32.h>

HANDLE FindProcess(LPTSTR pszProcessName)
{
	HANDLE hProcess = 0;
	PROCESSENTRY32 Entry;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		int ierr = GetLastError();
		CHAR ch[7];
		itoa(ierr, ch, 10);
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ierr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0,NULL );
		MessageBox(NULL, (LPCSTR)lpMsgBuf, "", MB_OKCANCEL);
		LocalFree(lpMsgBuf);
		return 0;
	}
	//исчем
	Entry.dwSize = sizeof(Entry);
    if (!Process32First(hSnapshot, &Entry))
		return 0;
    do
    {
        LPTSTR pszProcessName1 = NULL;
        if (Entry.dwSize > offsetof(PROCESSENTRY32, szExeFile))
        {
            pszProcessName1 = _tcsrchr(Entry.szExeFile, _T('\\'));
            if (pszProcessName1 == NULL)
                pszProcessName1 = Entry.szExeFile;
//сравниваем по имени
			if (strcmp(pszProcessName1,pszProcessName)==0)
			{
//получаем список потоков процесса
				HANDLE hProcessSn = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, Entry.th32ProcessID);
				if (hProcessSn == INVALID_HANDLE_VALUE)
						return 0;
//ищем первый поток процесса
				THREADENTRY32 te = {sizeof(te)};
				BOOL bRes = Thread32First(hProcessSn, &te);
				for (; bRes; bRes=Thread32Next(hProcessSn, &te))
				{
//поток этого процесса?
					if (te.th32OwnerProcessID == Entry.th32ProcessID)
					{
						HANDLE hProcess1 = OpenThread(SYNCHRONIZE, FALSE, te.th32ThreadID);
						if (hProcess1!=NULL)
						{
							hProcess = hProcess1;
						    CloseHandle(hProcessSn);
						    CloseHandle(hSnapshot);
							return hProcess;
						}
					}
				}
			}
        }
        Entry.dwSize = sizeof(Entry);
    }
    while (Process32Next(hSnapshot, &Entry));
    CloseHandle(hSnapshot);
	return hProcess;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hProcess = FindProcess(_T("TestCharMsg.exe"));
	if (hProcess)
	{
		WaitForSingleObject(hProcess, INFINITE);
		//как только закончили, значит процесс упал
		return 0;
	}
	return 0;
}

