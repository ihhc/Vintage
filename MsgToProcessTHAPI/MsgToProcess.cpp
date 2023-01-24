// MsgToProcess.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MsgToProcess.h"
#include <iostream>
#include <tchar.h>
#include <tlhelp32.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// This is an example of an exported function.
extern "C" __declspec( dllexport ) __cdecl void fnMsgToProcess(char* lpszProcessName, int ch)
{
	HANDLE hProcess = 0;
	PROCESSENTRY32 Entry;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
		return;
	//исчем
	Entry.dwSize = sizeof(Entry);
    if (!Process32First(hSnapshot, &Entry))
		return;
    do
    {
        LPTSTR pszProcessName1 = NULL;
        if (Entry.dwSize > offsetof(PROCESSENTRY32, szExeFile))
        {
            pszProcessName1 = _tcsrchr(Entry.szExeFile, _T('\\'));
            if (pszProcessName1 == NULL)
                pszProcessName1 = Entry.szExeFile;
//сравниваем по имени
			if (strcmp(pszProcessName1,lpszProcessName)==0)
			{
//получаем список потоков процесса
				HANDLE hProcessSn = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, Entry.th32ProcessID);
				if (hProcessSn == INVALID_HANDLE_VALUE)
						return;
//ищем первый поток процесса
				THREADENTRY32 te = {sizeof(te)};
				BOOL bRes = Thread32First(hProcessSn, &te);
				for (; bRes; bRes=Thread32Next(hProcessSn, &te))
				{
//поток этого процесса?
					if (te.th32OwnerProcessID == Entry.th32ProcessID)
					{
						PostThreadMessage(te.th32ThreadID, WM_CHAR, ch, ch);
					    CloseHandle(hProcessSn);
						CloseHandle(hSnapshot);
						return;
					}
				}
			}
        }
        Entry.dwSize = sizeof(Entry);
    }
    while (Process32Next(hSnapshot, &Entry));
    CloseHandle(hSnapshot);	
}
