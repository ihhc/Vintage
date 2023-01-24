// SMTPSenderCon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include ".\SMTP\Smtp.h"

//server, port, user, password, sender name, sender address, recipient address, subject, message text
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc<10)
	{
		printf(_T("smtp server, port, user, password, sender name, sender address, recipient address, subject, message text"));
		return 0;
	}
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0),&wsa);
	CSmtp mail;
	CSmtpMessage msg;
	msg.MimeType = mimeRelated;
	msg.Recipient.Address = argv[7];
	CSmtpMessageBody body;
	body.TransferEncoding = encode8Bit;
	body.Encoding = _T("text/html");
	TCHAR tchb[1024]; tchb[1023] = '\0';
	strcpy(tchb, argv[9]);
	for (int i=10; i<argc; i++)
	{	
		strcat(tchb, " "); 
		strcat(tchb, argv[i]); 
	}
	body = tchb;
	msg.Message.Add(body);
	msg.Sender.Name = argv[5];
	msg.Sender.Address = argv[6];
	msg.Subject = argv[8];
	mail.m_strUser = argv[3];
	mail.m_strPass = argv[4];
	mail.m_wSmtpPort = htons(atoi(argv[2]));
	if (mail.Connect(argv[1]))
	{	
		if (mail.SendMessage(msg)==0)
			printf(_T("sent"));	
	}
	mail.Close();
	WSACleanup();
	return 0;
}

