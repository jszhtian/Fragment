// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "detours.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "crc32.h"
#include <iomanip>
#pragma comment(lib, "detours.lib")
using namespace std;

ofstream flog("SCR.txt");

unordered_map<DWORD, wstring> SCRList;

unordered_map<DWORD, wstring> REPList;


PVOID g_pOldMultiByteToWideChar = NULL;
typedef int(WINAPI *PfuncMultiByteToWideChar)(
	_In_      UINT   CodePage,
	_In_      DWORD  dwFlags,
	_In_      LPCSTR lpMultiByteStr,
	_In_      int    cbMultiByte,
	_Out_opt_ LPWSTR lpWideCharStr,
	_In_      int    cchWideChar);

typedef HFONT(WINAPI* fnCreateFontIndirectW)(LOGFONTW *lplf);
fnCreateFontIndirectW pCreateFontIndirectW;

char* wtocUTF(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_UTF8, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度
	char *out = new char[dwMinSize];
	WideCharToMultiByte(CP_UTF8, NULL, str, -1, out, dwMinSize, NULL, FALSE);//转换
	return out;
}

char* wtocGBK(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(936, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度
	char *out = new char[dwMinSize];
	WideCharToMultiByte(936, NULL, str, -1, out, dwMinSize, NULL, FALSE);//转换
	return out;
}

LPWSTR ctowUTF(char* str)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); //计算长度
	LPWSTR out = new wchar_t[dwMinSize];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, out, dwMinSize);//转换
	return out;
}


static void make_console() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	cout << "Open Console Success!" << endl;
}

HFONT WINAPI newCreateFontIndirectW(LOGFONTW *lplf)
{
	__asm
	{
		pushad
	}

	LOGFONTW lf;
	memcpy(&lf, lplf, sizeof(LOGFONTW));
	wcscpy(lf.lfFaceName, L"微软雅黑");
	lf.lfCharSet = ANSI_CHARSET;
	__asm
	{
		popad
	}

	return pCreateFontIndirectW(&lf);
}

int WINAPI NewMultiByteToWideChar(UINT cp, DWORD dwFg, LPCSTR lpMBS, int cbMB, LPWSTR lpWCS, int ccWC)
{
	__asm
	{
		pushad
	}
	CRC32 crc;
	int ret = 0;
	ret = ((PfuncMultiByteToWideChar)g_pOldMultiByteToWideChar)(932, dwFg, lpMBS, cbMB, lpWCS, ccWC);
	if (lpWCS != 0)
	{
		//flog << (DWORD)str << endl;
		//flog << wtoc(str) << endl;
		/*
		if ((USHORT)*lpWCS > 0x7f)
		{
			if (wcscmp(lpWCS,L"自転車のロードレースは過酷なスポーツとして知られる。%K%P")==0)
			{
				wcscpy(lpWCS, L"自行车zhongwenceshi.jpg测试中文中%K%P");
			}
		}
		*/
		if ((USHORT)*lpWCS > 0x7f)
		{
			wstring wstr = lpWCS;
			DWORD strcrc = crc.Calc((char*)lpWCS, ret);
			auto scitr = REPList.find(strcrc);
			if (scitr!= REPList.end())
			{
				wcscpy(lpWCS, scitr->second.c_str());
				ret = scitr->second.length();
				cout << "REPLACE:" << "0x" << hex << scitr->first << wtocGBK(scitr->second.c_str()) << endl;
			}
			else
			{
				auto itr = SCRList.find(strcrc);
				if (itr == SCRList.end())
				{
					flog << "0x" << hex << setw(8) << setfill('0') << strcrc << "|" << wtocUTF(wstr.c_str()) << endl;
					SCRList.insert(pair<DWORD, wstring>(strcrc, wstr));
				}
			}
			
		}
		if ((USHORT)*lpWCS == 0x25)
		{
			wstring wstr = lpWCS;
			DWORD strcrc = crc.Calc((char*)lpWCS, ret);
			auto scitr = REPList.find(strcrc);
			if (scitr != REPList.end())
			{
				wcscpy(lpWCS, scitr->second.c_str());
				ret = scitr->second.length();
				cout << "REPLACE:" << "0x" << hex << scitr->first << wtocGBK(scitr->second.c_str()) << endl;
			}
			else
			{
				auto itr = SCRList.find(strcrc);
				if (itr == SCRList.end())
				{
					flog << "0x" << hex << setw(8) << setfill('0') << strcrc << "|" << wtocUTF(wstr.c_str()) << endl;
					SCRList.insert(pair<DWORD, wstring>(strcrc, wstr));
				}
			}
			
		}
		
		
	}
	__asm
	{
		popad
	}
	return ret;
}

void LoadStringMap()
{
	REPList.reserve(0x100000);
	ifstream fin("REP.txt");
	const int LineMax = 0x400;
	char str[LineMax];
	if (fin.is_open())
	{
		cout << "LOAD:"  << "Load start." << endl;
		int counter = 0;
		while (fin.getline(str,LineMax))
		{
			auto wtmp = ctowUTF(str);
			wstring wline = wtmp;
			wstring crcval = wline.substr(2, 8);
			wstring wstr = wline.substr(11);
			DWORD crc = wcstoul(crcval.c_str(), NULL, 16);
			//cout << "LOAD:" <<"0x"<<hex<<crc<< wtocGBK(wstr.c_str()) << endl;
			REPList.insert(pair<DWORD, wstring>(crc, wstr));
			counter++;
		}
		cout << "LOAD:" << counter << " Recoders." << endl;
	}
	else
	{
		MessageBox(0, L"File Open fail!", L"Error", MB_OK);
	}
}

void BeginDetour() 
{
	g_pOldMultiByteToWideChar = DetourFindFunction("Kernel32.dll", "MultiByteToWideChar");
	pCreateFontIndirectW = (fnCreateFontIndirectW)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "CreateFontIndirectW");
	DetourTransactionBegin();
	DetourAttach(&g_pOldMultiByteToWideChar, NewMultiByteToWideChar);
	DetourAttach((void**)&pCreateFontIndirectW, newCreateFontIndirectW);
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
		SCRList.reserve(0x100000);
		make_console();
		LoadStringMap();
		if (!flog.is_open())
		{
			MessageBox(0, L"File Open fail!", L"Error", MB_OK);
		}
		BeginDetour();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}