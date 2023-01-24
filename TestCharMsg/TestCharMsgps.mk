
TestCharMsgps.dll: dlldata.obj TestCharMsg_p.obj TestCharMsg_i.obj
	link /dll /out:TestCharMsgps.dll /def:TestCharMsgps.def /entry:DllMain dlldata.obj TestCharMsg_p.obj TestCharMsg_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del TestCharMsgps.dll
	@del TestCharMsgps.lib
	@del TestCharMsgps.exp
	@del dlldata.obj
	@del TestCharMsg_p.obj
	@del TestCharMsg_i.obj
