// コンパイル時にChromeとビット長(x86/x64)を合わせる必要がある
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>

#define BufLen 32768

BOOL __fastcall GetCmdExW(HANDLE hProcess, LPWSTR pszBuffer, int bufferLength)
{
	NTSTATUS status;
	PROCESS_BASIC_INFORMATION pbi;
	PEB peb;
	RTL_USER_PROCESS_PARAMETERS upp;
	
	status = NtQueryInformationProcess(
		hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
	if (!NT_SUCCESS(status) || !pbi.PebBaseAddress) {
		return FALSE;
	}
	
	if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), NULL)) {
		return FALSE;
	}
	
	if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &upp, sizeof(upp), NULL)) {
		return FALSE;
	}
	
	if (!upp.CommandLine.Length) {
		return FALSE;
	}
	
	if (!ReadProcessMemory(hProcess, upp.CommandLine.Buffer, pszBuffer, upp.CommandLine.Length, NULL)) {
		return FALSE;
	}
	
	return TRUE;
}

int WinMainCRTStartup()
{
	HANDLE hSnapshot, hProcess;
	PROCESSENTRY32 pe;
	WCHAR *pCmdLn, *buf;
	BOOL bAll;
	HMODULE hNtDll;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return -1;

	pCmdLn = GetCommandLineW();
	bAll = wcsstr(pCmdLn, L" -a") != 0;

	buf = (WCHAR*)GlobalAlloc(GMEM_FIXED, BufLen);

	pe.dwSize = sizeof(pe);
	if (Process32First(hSnapshot, &pe))
	{
		do
		{
			if (!lstrcmp("chrome.exe", pe.szExeFile))
			{
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
				if (hProcess)
				{
					if (GetCmdExW(hProcess, buf, BufLen)
						&& (!wcsstr(buf, L"--extension-process") || bAll)
						&&  !wcsstr(buf, L"--type=gpu-process")
						&&   wcsstr(buf, L"--type=renderer"))
						TerminateProcess(hProcess, -1);
					CloseHandle(hProcess);
				}
			}
		}
		while(Process32Next(hSnapshot, &pe));
	}

	//GlobalFree(buf);
	//CloseHandle(hSnapshot);
	return 0;
}
