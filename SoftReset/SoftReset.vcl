<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: SoftReset - Win32 (WCE ARM) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\IGORBO~1\LOCALS~1\Temp\RSP20.tmp" with contents
[
/nologo /W3 /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_AFXDLL" /FR"ARMRel/" /Fp"ARMRel/SoftReset.pch" /Yu"stdafx.h" /Fo"ARMRel/" /Oxs /MC /c 
"C:\PocketPC\SoftReset\SoftReset.cpp"
"C:\PocketPC\SoftReset\SoftResetDlg.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\IGORBO~1\LOCALS~1\Temp\RSP20.tmp" 
Creating temporary file "C:\DOCUME~1\IGORBO~1\LOCALS~1\Temp\RSP21.tmp" with contents
[
/nologo /W3 /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_AFXDLL" /FR"ARMRel/" /Fp"ARMRel/SoftReset.pch" /Yc"stdafx.h" /Fo"ARMRel/" /Oxs /MC /c 
"C:\PocketPC\SoftReset\StdAfx.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\IGORBO~1\LOCALS~1\Temp\RSP21.tmp" 
<h3>Output Window</h3>
Compiling...
StdAfx.cpp
Compiling...
SoftReset.cpp
SoftResetDlg.cpp
Generating Code...
Creating temporary file "C:\DOCUME~1\IGORBO~1\LOCALS~1\Temp\RSP22.tmp" with contents
[
/nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"wWinMainCRTStartup" /incremental:no /pdb:"ARMRel/SoftReset.pdb" /out:"ARMRel/SoftReset.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
.\ARMRel\SoftReset.obj
.\ARMRel\SoftResetDlg.obj
.\ARMRel\StdAfx.obj
.\ARMRel\SoftReset.res
]
Creating command line "link.exe @C:\DOCUME~1\IGORBO~1\LOCALS~1\Temp\RSP22.tmp"
<h3>Output Window</h3>
Linking...
Creating command line "bscmake.exe /nologo /o"ARMRel/SoftReset.bsc"  .\ARMRel\StdAfx.sbr .\ARMRel\SoftReset.sbr .\ARMRel\SoftResetDlg.sbr"
Creating browse info file...
<h3>Output Window</h3>



<h3>Results</h3>
SoftReset.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
