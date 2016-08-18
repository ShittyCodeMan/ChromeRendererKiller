fcr.exe: fcr.obj
	link fcr.obj /nologo /FIXED kernel32.lib user32.lib ntdll.lib
fcr.obj: fcr.c
	cl fcr.c /c /nologo /O1 /GS- /Oi-
