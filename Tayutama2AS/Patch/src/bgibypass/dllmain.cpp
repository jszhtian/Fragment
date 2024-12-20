// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include <string>


typedef LCID (WINAPI* fnGetUserDefaultLCID)(void);
typedef LANGID (WINAPI* fnGetSystemDefaultLangID)(void);
typedef LANGID (WINAPI* fnGetSystemDefaultUILanguage)(void);
typedef bool (WINAPI* fnSetWindowTextA)(HWND hWnd, LPCSTR lpString);
typedef bool (WINAPI* fnTextOutA)(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cchString);
typedef HFONT(WINAPI* fnCreateFontA)(_In_ int     nHeight,
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
	_In_ LPCTSTR lpszFace);
typedef int (WINAPI* fnEnumFontFamiliesExA)(
	HDC           hdc,
	LPLOGFONTA    lpLogfont,
	FONTENUMPROCA lpProc,
	LPARAM        lParam,
	DWORD         dwFlags
	);
fnCreateFontA pCreateFontA;
fnGetSystemDefaultLangID pGetSystemDefaultLangID;
fnGetSystemDefaultUILanguage pGetSystemDefaultUILanguage;
fnGetUserDefaultLCID pGetUserDefaultLCID;
fnSetWindowTextA pSetWindowTextA;
fnTextOutA pTextOutA;
fnEnumFontFamiliesExA pEnumFontFamiliesExA;

std::wstring ctow936(const char *str)
{
	wchar_t* buffer = nullptr;
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(936, 0, (const char *)str, int(nu), NULL, 0);
		buffer = 0;
		buffer = new wchar_t[n];
		memset(buffer, 0, sizeof(wchar_t)*(n));
		MultiByteToWideChar(936, 0, (const char *)str, int(nu), buffer, int(n));
	}
	if (buffer!=nullptr)
	{
		auto ret = std::wstring(buffer);
		delete[] buffer;
		return ret;
	}
	else
	{
		return std::wstring();
	}
	
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
	fdwCharSet = GB2312_CHARSET;
	char buffer[0x100] = { 0 };
	memset(buffer, 0, 0x100);
	memcpy(buffer, lpszFace, strlen(lpszFace));
	const auto wcs = ctow936(buffer);
	return CreateFontW(nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, wcs.c_str());
}

LCID WINAPI newGetUserDefaultLCID()
{
	return 0x411;
}

LANGID WINAPI newGetSystemDefaultLangID()
{
	return 0x411;
}

LANGID WINAPI newGetSystemDefaultUILanguage()
{
	return 0x411;
}

bool WINAPI newSetWindowTextA(HWND hw, LPCSTR lps)
{
	const unsigned char chk[] = { 0x83, 0x5e, 0x83, 0x86, 0x83, 0x5e, 0x83, 0x7d, 0x82, 0x51 };
	if (memcmp(lps, chk, 10) == 0)
	{
		const wchar_t newtitle[] = L"游魂２-After Stories-";
		return SetWindowTextW(hw, newtitle);
	}
	else
	{
		return ((fnSetWindowTextA)pSetWindowTextA)(hw, lps);
	}
}

int WINAPI newEnumFontFamiliesExA(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpProc, LPARAM lParam, DWORD dwFlags)
{
	LOGFONTA Lfont;
	memcpy(&Lfont, lpLogfont, sizeof(LOGFONTA));
	Lfont.lfCharSet = GB2312_CHARSET;
	memcpy(lpLogfont, &Lfont, sizeof(LOGFONTA));
	return ((fnEnumFontFamiliesExA)pEnumFontFamiliesExA)(hdc, lpLogfont, lpProc, lParam, dwFlags);
}


bool WINAPI newTextOutA(HDC hw, int x, int y, LPCSTR lpStr, int len)
{
	char buffer[2] = { 0 };
	memcpy(buffer, lpStr, 2);
	unsigned int val = *(unsigned int*)buffer;
	bool is_SJIS = false;
	wchar_t replace_wcr[2] = { 0x0 };
	switch (val)
	{
	case 0x9999:
		wcscpy(replace_wcr , L"♪");
		is_SJIS = true;
		break;
	case 0xA485:
		wcscpy(replace_wcr, L"・");
		is_SJIS = true;
		break;
	case 0x85A6:
		wcscpy(replace_wcr, L"∞");
		is_SJIS = true;
		break;
	default:
		is_SJIS = false;
		break;
	}
	if (is_SJIS)
	{

		return TextOutW(hw, x, y, replace_wcr, wcsnlen(replace_wcr,2));
	}
	else
	{
		return ((fnTextOutA)pTextOutA)(hw, x, y, lpStr, len);
	}
}

void SetHook()
{
	pCreateFontA = (fnCreateFontA)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "CreateFontA");
	pEnumFontFamiliesExA = (fnEnumFontFamiliesExA)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "EnumFontFamiliesExA");
	pGetSystemDefaultLangID = (fnGetSystemDefaultLangID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "GetSystemDefaultLangID");
	pGetSystemDefaultUILanguage=(fnGetSystemDefaultUILanguage)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "GetSystemDefaultUILanguage");
	pGetUserDefaultLCID=(fnGetUserDefaultLCID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "GetUserDefaultLCID");
	pSetWindowTextA=(fnSetWindowTextA)GetProcAddress(GetModuleHandle(L"User32.dll"), "SetWindowTextA");
	pTextOutA = (fnTextOutA)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "TextOutA");

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((void**)&pCreateFontA, newCreateFontA);
	DetourAttach((void**)&pGetSystemDefaultLangID, newGetSystemDefaultLangID);
	DetourAttach((void**)&pGetSystemDefaultUILanguage, newGetSystemDefaultUILanguage);
	DetourAttach((void**)&pGetUserDefaultLCID, newGetUserDefaultLCID);
	DetourAttach((void**)&pSetWindowTextA, newSetWindowTextA);
	DetourAttach((void**)&pEnumFontFamiliesExA, newEnumFontFamiliesExA);
	DetourAttach((void**)&pTextOutA, newTextOutA);
	if (DetourTransactionCommit() != NO_ERROR)
	{
		MessageBoxW(NULL, L"Hook目标函数失败", L"严重错误", MB_OK | MB_ICONWARNING);
	}
}

void UnsetHook()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach((void**)&pCreateFontA, newCreateFontA);
	DetourDetach((void**)&pGetSystemDefaultLangID, newGetSystemDefaultLangID);
	DetourDetach((void**)&pGetSystemDefaultUILanguage, newGetSystemDefaultUILanguage);
	DetourDetach((void**)&pGetUserDefaultLCID, newGetUserDefaultLCID);
	DetourDetach((void**)&pSetWindowTextA, newSetWindowTextA);
	DetourDetach((void**)&pEnumFontFamiliesExA, newEnumFontFamiliesExA);
	DetourDetach((void**)&pTextOutA, newTextOutA);
	DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		SetHook();
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		UnsetHook();
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}