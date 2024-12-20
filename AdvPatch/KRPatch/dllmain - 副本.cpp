// dllmain.cpp :  定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "TransText.h"
using namespace std;
bool debug = true;

static int(WINAPI *OldMultiByteToWideChar)(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) = MultiByteToWideChar;
static int(WINAPI *OldWideCharToMultiByte)(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar) = WideCharToMultiByte;
static HFONT(WINAPI *OldCreateFontIndirectW)(CONST LOGFONTW *lplf) = CreateFontIndirectW;

TransText translatetext;
LPVOID pMap = nullptr;
TextStruct *textbuf = nullptr;
HANDLE hFile = INVALID_HANDLE_VALUE;
HANDLE hFileMap = INVALID_HANDLE_VALUE;

char *wtoc(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = OldWideCharToMultiByte(CP_ACP, NULL, str, -1, NULL, 0, NULL, FALSE); 
	char *bbb = new char[dwMinSize];
	OldWideCharToMultiByte(CP_ACP, NULL, str, -1, bbb, dwMinSize, NULL, FALSE);
	return bbb;
}

static void make_console()
{
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

	return OldCreateFontIndirectW(&lf);
}

static volatile int thread_searching = 0;
std::mutex mut;
void FindandReplaceStr(LPWSTR lpWCS, int *ret)
{
	//cout << "Start Search" << endl;
	//while (thread_searching) {}
	//thread_searching = 1;
	mut.lock();
	if ((USHORT)*lpWCS == 0x25)
	{
		goto startsearch;
	}
	if ((USHORT)*lpWCS == 0x7B)
	{
		goto startsearch;
	}

	if ((USHORT)*lpWCS > 0x7f)
	{
		goto startsearch;
	}
	goto exiting;
startsearch:
	auto res = translatetext.searchtext(lpWCS);
	if (res != nullptr)
	{
		if (debug)
		{
			cout << "Replace text:" << wtoc(res) << endl;
		}

		*ret = wcslen(res);
		wcscpy(lpWCS, res);
	}
	else
	{
		if (debug)
		{
			cout << "Skip text:" << wtoc(lpWCS) << endl;
		}
	}
exiting:
	mut.unlock();
	//thread_searching = 0;
}

int WINAPI newMultiByteToWideChar(UINT cp, DWORD dwFg, LPCSTR lpMBS, int cbMB, LPWSTR lpWCS, int ccWC)
{
	__asm
	{
		pushfd
		pushad
	}

	int ret = 0;
	ret = OldMultiByteToWideChar(932, dwFg, lpMBS, cbMB, lpWCS, ccWC);
	if (lpWCS != 0)
	{
		wchar_t chk[] = L"■";
		if (memcmp(lpWCS, chk, 2)==0)
		{
			ret= OldMultiByteToWideChar(936, dwFg, lpMBS, cbMB, lpWCS, ccWC);
			memcpy(lpWCS, chk, 2);
		}
		else
		{
			FindandReplaceStr(lpWCS, &ret);
		}
		
	}
	else
	{
		ret = 0x100;
	}
	__asm
	{
		popad
		popfd
	}
	return ret;
}

bool wc2mbchk(const WCHAR* rawstr)
{
	DWORD dwMinSize;
	dwMinSize = OldWideCharToMultiByte(932, NULL, rawstr, -1, NULL, 0, NULL, FALSE);
	char *bbb = new char[dwMinSize];
	OldWideCharToMultiByte(932, NULL, rawstr, -1, bbb, dwMinSize, NULL, FALSE);
	wchar_t* buffer;
	size_t nu = strlen(bbb);
	size_t n = (size_t)OldMultiByteToWideChar(932, 0, (const char *)bbb, int(nu), NULL, 0);
	buffer = new wchar_t[n + 1];
	memset(buffer, 0, sizeof(wchar_t)*(n + 1));
	OldMultiByteToWideChar(932, 0, (const char *)bbb, int(nu), buffer, int(n));
	auto chkresult = wcscmp(buffer, rawstr);
	delete bbb;
	delete buffer;
	if (chkresult == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int WINAPI newWideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	__asm
		{
		pushad
		}
	int ret;
	if (wc2mbchk(lpWideCharStr))
	{
		ret = OldWideCharToMultiByte(932, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
	}
	else
	{
		ret = OldWideCharToMultiByte(936, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
	}
	

	__asm
	{
		popad
	}

	return ret;
}
/*
int CountLines(char *filename)
{
ifstream ReadFile;
int n = 0;
string temp;
ReadFile.open(filename, ios::in);
if (ReadFile.fail())
{
return 0;
}
else
{
while (getline(ReadFile, temp))
{
n++;
}
ReadFile.close();
return n;
}

}
void LoadStringMap()
{
auto strnum = CountLines("REP.txt");
textbuf = new TextStruct[strnum];
cout << "LOAD:" << strnum << " lines from file" << endl;
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
textbuf[counter].crc32val = crc;
wcscpy(textbuf[counter].str, wstr.c_str());
counter++;
}
cout << "LOAD:" << counter << " Recoders." << endl;
translatetext.setStorage(textbuf);
translatetext.setTableLength(counter);
}
else
{
MessageBox(0, L"File Open fail!", L"Error", MB_OK);
}
}
*/

void LoadStringMap()
{
	DWORD dwBytesRead, dwBytesToRead;
	hFile = CreateFileW(L"Comp.pck", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(NULL, L"CreateFile fail!", L"严重错误", MB_OK);
	}
	DWORD filesize = GetFileSize(hFile, NULL);
	head *hdr = new head;
	dwBytesRead = sizeof(head);
	ReadFile(hFile, hdr, dwBytesRead, &dwBytesToRead, NULL);
	hFileMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, filesize, NULL);
	if (hFileMap == NULL)
	{
		MessageBoxW(NULL, L"CreateFileMapping fail!", L"严重错误", MB_OK);
	}
	pMap = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, filesize);
	if (pMap == NULL)
	{
		MessageBoxW(NULL, L"MapViewOfFile fail!", L"严重错误", MB_OK);
	}
	textbuf = (TextStruct *)((char *)pMap + 0x10);
	translatetext.setStorage(textbuf);
	translatetext.setTableLength(hdr->Length);
	delete hdr;
}

void BeginDetour()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID &)OldMultiByteToWideChar, newMultiByteToWideChar);
	DetourAttach(&(PVOID &)OldCreateFontIndirectW, newCreateFontIndirectW);
	DetourAttach(&(PVOID &)OldWideCharToMultiByte, newWideCharToMultiByte);
	if (DetourTransactionCommit() != NO_ERROR)
	{
		MessageBoxW(NULL, L"Hook目标函数失败", L"严重错误", MB_OK | MB_ICONWARNING);
	}
}

/*
void Unload()
{
DetourTransactionBegin();
DetourDetach(&(PVOID&)OldMultiByteToWideChar, newMultiByteToWideChar);
DetourDetach(&(PVOID&)OldCreateFontIndirectW, newCreateFontIndirectW);
DetourDetach(&(PVOID&)OldWideCharToMultiByte, newWideCharToMultiByte);
DetourTransactionCommit();
UnmapViewOfFile(pMap);
textbuf = nullptr;
CloseHandle(hFileMap);
hFileMap = INVALID_HANDLE_VALUE;
CloseHandle(hFile);
hFile = INVALID_HANDLE_VALUE;
}
*/

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (debug)
		{
			make_console();
		}

		LoadStringMap();
		//must load texts before Hook!
		BeginDetour();
	case DLL_THREAD_ATTACH:
		//Unload();
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void)
{
	return;
}