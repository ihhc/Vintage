#include <stdafx.h>
#include <comdef.h>

#define XP_NOERROR              0
#define XP_ERROR                1
#define MAXCOLNAME				25
#define MAXNAME					25
#define MAXTEXT					255

#ifdef __cplusplus
extern "C" {
#endif

RETCODE __declspec(dllexport) xp_tcpproc(SRV_PROC *srvproc);

#ifdef __cplusplus
}
#endif

int connectex(SOCKET s, const struct sockaddr *name,
                            int namelen, long timeout)
{
    // As connect() but with timeout setting.
    int            rc = 0;
    ULONG          ulB;
    struct timeval Time;
    fd_set         FdSet;

    ulB = TRUE; // Set socket to non-blocking mode
    ioctlsocket(s, FIONBIO, &ulB);

    if (connect(s, name, sizeof(SOCKADDR)) == SOCKET_ERROR) {

        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            // now wait for the specified time
            FD_ZERO(&FdSet);
            FD_SET(s, &FdSet);

            Time.tv_sec  = timeout / 1000L;
            Time.tv_usec = (timeout % 1000) * 1000;
            rc = select(0, NULL, &FdSet, NULL, &Time);

        }
    }

    ulB = FALSE; // Restore socket to blocking mode
    ioctlsocket(s, FIONBIO, &ulB);

    return (rc > 0) ? 0 : SOCKET_ERROR;
}

RETCODE __declspec(dllexport) xp_tcpproc(SRV_PROC *srvproc)
{
	DBCHAR spText[MAXTEXT];
	// Check that there are the correct number of parameters.
	if ( srv_rpcparams(srvproc) != 3 )
	{
		// If there is not exactly one parameter, send an error to the client.
		_snprintf(spText, MAXTEXT, "ERROR. You need to pass 3 parameters.");
		srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);
		// Signal the client that we are finished.
		srv_senddone(srvproc, SRV_DONE_ERROR, (DBUSMALLINT)0, (DBINT)0);
		return XP_ERROR;
	}

	BOOL bNull;
	BYTE bType;
	ULONG uLen;
	ULONG uMaxLen;

	// Get the info about the parameter.  
	// Note pass NULL for the pbData parameter to get information rather than the parameter itself.
	srv_paraminfo(srvproc, 1, &bType, &uMaxLen, &uLen, NULL, &bNull); 
	BYTE* sHost = new BYTE[uLen+1];
	sHost[uLen] = '\0';
	memset(sHost, '\0', uLen);
	srv_paraminfo(srvproc, 1, &bType, &uMaxLen, &uLen, sHost, &bNull); 

	srv_paraminfo(srvproc, 2, &bType, &uMaxLen, &uLen, NULL, &bNull); 
	BYTE* Data = new BYTE[uLen];
	memset(Data, '\0', uLen);
	srv_paraminfo(srvproc, 2, &bType, &uMaxLen, &uLen, Data, &bNull); 
	long lPort = (long)*Data;
	delete [] Data;


	srv_paraminfo(srvproc, 3, &bType, &uMaxLen, &uLen, NULL, &bNull); 
	BYTE* sText = new BYTE[uLen+1];
	sText[uLen] = '\0';
	memset(sText, '\0', uLen);
	srv_paraminfo(srvproc, 3, &bType, &uMaxLen, &uLen, sText, &bNull); 

	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0),&wsa);

	SOCKET   hSocket;
	
	hSocket = INVALID_SOCKET;
	SOCKADDR_IN addr;
	int ihostlen = strlen((char*)sHost);
	LPHOSTENT pHost = gethostbyname((char*)sHost);//, s.GetLength(), AF_INET);
	if (!pHost)	
	{ 
		_snprintf(spText, MAXTEXT, "HOST ERROR.");
		srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);
		srv_senddone(srvproc, SRV_DONE_ERROR, (DBUSMALLINT)0, (DBINT)0);
		return XP_ERROR;
	}
	addr.sin_addr = *((LPIN_ADDR)*pHost->h_addr_list);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(lPort);

	hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	if (hSocket == INVALID_SOCKET) 
	{
		_snprintf(spText, MAXTEXT, "Socket creation failed.");
		srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);
		srv_senddone(srvproc, SRV_DONE_ERROR, (DBUSMALLINT)0, (DBINT)0);
		return XP_ERROR;
	}
	if (connectex(hSocket,(LPSOCKADDR)&addr,sizeof(addr), 600000) == SOCKET_ERROR)
	{
		int ierr = WSAGetLastError();
		CHAR ch[7];
		itoa(ierr, ch, 10);
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ierr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0,NULL );
		_snprintf(spText, MAXTEXT, (LPCSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
		srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);
		srv_senddone(srvproc, SRV_DONE_ERROR, (DBUSMALLINT)0, (DBINT)0);
		return XP_ERROR;
	}

//-------------send data-------------------------------
	fd_set	fdWrite  = { 0 };
	TIMEVAL	stTime;
	TIMEVAL	*pstTime = NULL;
	SOCKET s = (SOCKET) hSocket;
	if ( !FD_ISSET( s, &fdWrite ) )
		FD_SET( s, &fdWrite );
	int res = select( s+1, NULL, &fdWrite, NULL, pstTime );
	if (res <= 0) 
	{
		int ierr = WSAGetLastError();
		CHAR ch[7];
		itoa(ierr, ch, 10);
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ierr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0,NULL );
		_snprintf(spText, MAXTEXT, (LPCSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
		srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);
		srv_senddone(srvproc, SRV_DONE_ERROR, (DBUSMALLINT)0, (DBINT)0);
		return XP_ERROR;
	}
	res = send( s, (LPCSTR)sText, strlen((char*)sText), 0);
	if (res <= 0) 
	{
		int ierr = WSAGetLastError();
		CHAR ch[7];
		itoa(ierr, ch, 10);
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ierr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0,NULL );
		_snprintf(spText, MAXTEXT, (LPCSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
		srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);
		srv_senddone(srvproc, SRV_DONE_ERROR, (DBUSMALLINT)0, (DBINT)0);
		return XP_ERROR;
	}
//------------
	if (hSocket != INVALID_SOCKET)
	{
		shutdown(hSocket,2);
		closesocket(hSocket);
	}
	_snprintf(spText, MAXTEXT, "SUCCEEDED");
	srv_sendmsg( srvproc, SRV_MSG_INFO,	0,(DBTINYINT)0,	(DBTINYINT)0,NULL,0,0,spText,SRV_NULLTERM);

	// Signal the client that we are finished.
	srv_senddone(srvproc, SRV_DONE_COUNT, (DBUSMALLINT)0, (DBINT)1);

	// Tidy up.
	delete []sHost;
	delete []sText;

	WSACleanup();
	return XP_NOERROR ;
}

