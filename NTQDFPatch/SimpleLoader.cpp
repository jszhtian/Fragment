#pragma comment(lib, "BlackBone.lib")
#include <iostream>
#include <Windows.h>
#include <BlackBone/Process/Process.h>
#include <BlackBone/Process/ProcessModules.h>

#define BUFSIZE 4096

wchar_t clp_DllPath[BUFSIZE];
wchar_t clp_ExePath[BUFSIZE];
bool GetFullPath(wchar_t* RelPath, wchar_t* lp_FullPath)
{
	int retval = 0;
	ZeroMemory(lp_FullPath, BUFSIZE*sizeof(wchar_t));
	retval=GetFullPathNameW(RelPath, BUFSIZE, lp_FullPath, NULL);
	if (retval == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

int wmain(int argc,wchar_t* argv[])
{
	if (argc != 2)
	{
		MessageBoxW(NULL, L"请把EXE拖拽到本程序上！", L"错误", MB_OK | MB_ICONERROR);
		return -1;
	}
	wchar_t DllName[] = L"NTQDFPatch.dll";
	if (!FindFirstFileExists(DllName, FALSE))
	{
		MessageBoxW(NULL, L"找不到DLL文件！", L"错误", MB_OK | MB_ICONERROR);
		return -1;
	}
	if (!GetFullPath(DllName, clp_DllPath))
	{
		MessageBoxW(NULL, L"获取DLL完整路径失败！", L"错误", MB_OK | MB_ICONERROR);
		return -1;
	}
	if (!GetFullPath(argv[1], clp_ExePath))
	{
		MessageBoxW(NULL, L"获取EXE完整路径失败！", L"错误", MB_OK | MB_ICONERROR);
		return -1;
	}

	blackbone::Process _Process;
	_Process.CreateAndAttach(clp_ExePath, true);
	//auto pThread = _Process.threads().getMostExecuted();
	//_Process.modules().Inject(clp_DllPath, pThread);
	_Process.modules().Inject(clp_DllPath);
	_Process.Resume();
	_Process.Detach();
	return 0;
}
