strip: fcr.exe
	@powershell.exe "$$i=[System.IO.File]::ReadAllBytes((Get-Item -Path '.\\').FullName+'\\fcr.exe');$$i[0x2]=0;$$i[0x3]=0;[byte[]]$$o=$$i[0x0..0xB]+,0*12+$$i[0x18..0x1D]+,0*14+$$i[0x2C..0x2F]+,0*8+$$i[0x38..0x3B]+@(4,0,0,0)*2+$$i[0x44..$$i.length];[System.IO.File]::WriteAllBytes((Get-Item -Path '.\\').FullName+'\\fcr.exe',$$o);"
fcr.exe: fcr.obj ntdll_lib.lib
	@link fcr.obj /out:fcr.exe /nologo /fixed /subsystem:windows /nodefaultlib /incremental:no /section:.text,w /merge:.data=.text /merge:.rdata=.text /align:16 /safeseh:no /emitpogophaseinfo /ignore:4108,4254 kernel32.lib user32.lib ntdll.lib ntdll_lib.lib
fcr.obj: fcr.c
	@cl fcr.c /c /nologo /O1 /GS- /Oi-
ntdll_lib.lib: ntdll_lib.obj
	@lib ntdll_lib.obj /nologo /nodefaultlib /def:ntdll_lib.def
ntdll_lib.obj: ntdll_lib.c
	@cl /LD ntdll_lib.c /c /nologo /O1 /GS- /Oi-
