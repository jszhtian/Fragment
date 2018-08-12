// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <iostream>
#include <mutex>
using namespace std;
#pragma warning(disable:4311)
#pragma warning(disable:4312)  
#pragma warning(disable:4996) 
typedef LSTATUS (WINAPI* Func_RegOpenKeyExA)(HKEY   hKey, LPCSTR lpSubKey, DWORD  ulOptions, REGSAM samDesired, PHKEY  phkResult);
PVOID p_RegOpenKeyEx = RegOpenKeyExA;
typedef bool (WINAPI* Func_SetWindowTextA)(HWND hWnd, LPCSTR lpString);
PVOID p_SetWindowTextA = SetWindowTextA;
typedef HFONT (WINAPI* Func_CreateFontIndirectA)(LOGFONTA *lplf);
PVOID p_CreateFontIndirectA = CreateFontIndirectA;
typedef BYTE* (__cdecl* Func_ReadCompFile)(char* PakName, char* FileName, BYTE* unk1, DWORD* origlength, DWORD* CompSize);
PVOID p_ReadCompFile;
typedef HFONT(WINAPI* Func_CreateFontA)(_In_ int     nHeight,
	_In_ int     nWidth,
	_In_ int     nEscapement,
	_In_ int     nOrientation,
	_In_ int     fnWeight,
	_In_ DWORD   fdwItalic,
	_In_ DWORD   fdwUnderline,
	_In_ DWORD   fdwStrikeOut,
	_In_ DWORD   fdwCharSet,
	_In_ DWORD   fdwOutputPrecision,
	_In_ DWORD   fdwClipPrecision,
	_In_ DWORD   fdwQuality,
	_In_ DWORD   fdwPitchAndFamily,
	_In_ LPCSTR lpszFace);
PVOID p_CreateFontA = CreateFontA;
LSTATUS WINAPI NewRegOpenKeyExA(HKEY hk,LPCSTR subkey,DWORD ulO,REGSAM sam,PHKEY phk)
{
	wchar_t buffer[256];
	if (subkey)
	{
		size_t nu = strlen(subkey);
		size_t n = (size_t)MultiByteToWideChar(932, 0, subkey, int(nu), NULL, 0);
		memset(buffer, 0, sizeof(wchar_t) * 256);
		MultiByteToWideChar(932, 0, subkey, int(nu), buffer, int(n));
	}
	return RegOpenKeyExW(hk, buffer, ulO, sam, phk);
}

HFONT WINAPI NewCreateFontIndirectA(LOGFONTA *lplf)
{
	LOGFONTA lf;
	memcpy(&lf, lplf, sizeof(LOGFONTA));
	strcpy(lf.lfFaceName, "微软雅黑");
	lf.lfCharSet = GB2312_CHARSET;
	return (Func_CreateFontIndirectA(p_CreateFontIndirectA))(&lf);
}

bool WINAPI NewSetWindowTextA(HWND hw, LPCSTR lps)
{
	wchar_t buffer[256];
	if (lps)
	{
		size_t nu = strlen(lps);
		size_t n = (size_t)MultiByteToWideChar(932, 0, lps, int(nu), NULL, 0);
		memset(buffer, 0, sizeof(wchar_t) * 256);
		MultiByteToWideChar(932, 0, lps, int(nu), buffer, int(n));
	}
	
	return SetWindowTextW(hw, buffer);

}

void memcopy(void* dest, void*src, size_t size)
{
	DWORD oldProtect;
	VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dest, src, size);
	DWORD oldProtect2;
	VirtualProtect(dest, size, oldProtect, &oldProtect2);
}

/*
 IDA disassembly Result@.text:00474C30
 byte *__cdecl ReadCompFile(const CHAR *PakName, const char *FileName, byte *a3, DWORD *OrigLength, DWORD *CompSize)
{
  DWORD v5; // esi@1
  byte *result; // eax@4
  HANDLE hFile; // eax@7
  DWORD dwFileSize; // ecx@9

  v5 = (DWORD)CompSize;
  if ( CompSize )
	*CompSize = 0;
  if ( FileName )
  {
	if ( PakName )<-Point!
	{
	  hFile = sub_4746D0(PakName, FileName, GENERIC_READ, 0, 0, &CompSize, (char *)&FileName);
	  if ( hFile == (HANDLE)INVALID_HANDLE_VALUE )
	  {
		result = 0;
	  }
	  else
	  {
		dwFileSize = (DWORD)CompSize;
		if ( v5 )
		  *(_DWORD *)v5 = CompSize;
		result = internal_readfile(hFile, a3, dwFileSize, (unsigned int *)OrigLength, (char *)FileName);
	  }
	}
	else
	{
	  result = sub_474630((int)FileName, (UINT)a3, OrigLength);
	}
  }
  else
  {
	result = 0;
  }
  return result;
}
 
 */
BYTE* __cdecl NewReadCompFile(char* pkg,char* fnm,BYTE* unk,DWORD* ori,DWORD* cmpsize)
{
	DWORD dwFileSize = 0;
	DWORD dwBytesRead = 0;
	char* FileBuf = 0;
	bool isReadSuccess = false;
	wchar_t buffer[256];
	if (fnm)
	{
		size_t nu = strlen(fnm);
		size_t n = (size_t)MultiByteToWideChar(932, 0, fnm, int(nu), NULL, 0);
		memset(buffer, 0, sizeof(wchar_t) * 256);
		MultiByteToWideChar(932, 0, fnm, int(nu), buffer, int(n));
	}

	auto hFile=CreateFile(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		goto defaultRouting;
	}
	dwFileSize = GetFileSize(hFile, 0);
	FileBuf=new char[dwFileSize];
	if ( FileBuf==0)
	{
		goto defaultRouting;
	}
	memset(FileBuf, 0, dwFileSize);
	isReadSuccess =ReadFile(hFile, FileBuf, dwFileSize, &dwBytesRead, NULL);
	CloseHandle(hFile);
	if (!isReadSuccess)
	{
		goto defaultRouting;
	}
	*ori = dwFileSize;
	return (BYTE*)FileBuf;
defaultRouting:
	return (Func_ReadCompFile(p_ReadCompFile))(pkg, fnm, unk, ori, cmpsize);

	
}

HFONT WINAPI newCreateFontA(_In_ int     nHeight,
	_In_ int     nWidth,
	_In_ int     nEscapement,
	_In_ int     nOrientation,
	_In_ int     fnWeight,
	_In_ DWORD   fdwItalic,
	_In_ DWORD   fdwUnderline,
	_In_ DWORD   fdwStrikeOut,
	_In_ DWORD   fdwCharSet,
	_In_ DWORD   fdwOutputPrecision,
	_In_ DWORD   fdwClipPrecision,
	_In_ DWORD   fdwQuality,
	_In_ DWORD   fdwPitchAndFamily,
	_In_ LPCSTR lpszFace)
{
	lpszFace = "微软雅黑";
	fdwCharSet = GB2312_CHARSET;
	return ((Func_CreateFontA)p_CreateFontA)(nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace);
}

void SJISBypass()
{
	DWORD baseAddr = (DWORD)GetModuleHandle(NULL);
	BYTE Patch1[] = { 0xFE };
	//cmp al,0x9F
	memcopy((void*)(baseAddr + 0x3ADD3), Patch1, sizeof(Patch1));
	memcopy((void*)(baseAddr + 0x3ADDB), Patch1, sizeof(Patch1));
	//cmp cl,0x9F
	memcopy((void*)(baseAddr + 0x29D1C), Patch1, sizeof(Patch1));
	memcopy((void*)(baseAddr + 0x29D26), Patch1, sizeof(Patch1));
	memcopy((void*)(baseAddr + 0x3ABE6), Patch1, sizeof(Patch1));
	memcopy((void*)(baseAddr + 0x3ABF0), Patch1, sizeof(Patch1));
	memcopy((void*)(baseAddr + 0x3BDD4), Patch1, sizeof(Patch1));
	memcopy((void*)(baseAddr + 0x3BDDE), Patch1, sizeof(Patch1));
}

void init()
{
	DWORD baseAddr = (DWORD)GetModuleHandle(NULL);
	p_ReadCompFile = (PVOID)(baseAddr + 0x74c30);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&p_RegOpenKeyEx, NewRegOpenKeyExA);
	DetourAttach(&p_SetWindowTextA, NewSetWindowTextA);
	DetourAttach(&p_CreateFontIndirectA, NewCreateFontIndirectA);
	DetourAttach(&p_CreateFontA, newCreateFontA);
	DetourAttach(&p_ReadCompFile, NewReadCompFile);
	if (DetourTransactionCommit() != NO_ERROR)
	{
		MessageBoxW(NULL, L"Hook目标函数失败", L"严重错误", MB_OK | MB_ICONWARNING);
	}
	SJISBypass();

}

static void make_console() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	std::cout << "Open Console Success!" << std::endl;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//make_console();
		init();
		//MessageBoxW(NULL, L"MSG", L"TEST", MB_OK);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C"  void dummy(void) {
	return;
}