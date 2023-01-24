// MsgToProcessTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void (*pFunction)(char*, int);

int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwnd = FindWindow(NULL, "TestCharMsg"); 
	if (hwnd != NULL) 
		SendMessage(hwnd,WM_KEYDOWN,'a','a'); 
/*
	HMODULE hLib = LoadLibrary("MsgToProcess.dll");
	(FARPROC&) pFunction = GetProcAddress(hLib, "fnMsgToProcess");
	pFunction("TestCharMsg.exe", 'a');
	FreeLibrary(hLib);
*/
	return 0;
}

