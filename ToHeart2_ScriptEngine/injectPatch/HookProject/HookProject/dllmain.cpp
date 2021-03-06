// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "detours.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include "crc32.h"
#include <iomanip>
#include <mutex>
#include <set>
#include <list>
using namespace std;
//ofstream flog("out.log");
static wchar_t wchar_convert_buf[0x1000];
static char sjis_convert_buf[0x1000];
wchar_t* ctow(const char *str)
{
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(932, 0, (const char *)str, int(nu), NULL, 0);
		memset(wchar_convert_buf, 0, (n + 1) * sizeof(wchar_t));
		MultiByteToWideChar(932, 0, (const char *)str, int(nu), wchar_convert_buf, int(n));
	}
	return wchar_convert_buf;

}
char* wtoc(const wchar_t* str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(932, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度  
	memset(sjis_convert_buf, 0, dwMinSize + 2);
	WideCharToMultiByte(932, NULL, str, -1, sjis_convert_buf, dwMinSize, NULL, FALSE);
	return sjis_convert_buf;
}

char bbb[0x1000];
char* wtocGBK(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_ACP, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度  
	memset(bbb, 0, dwMinSize + 2);
	WideCharToMultiByte(CP_OEMCP, NULL, str, -1, bbb, dwMinSize, NULL, FALSE);
	return bbb;
}
struct block {
	wchar_t Ori;
	wchar_t Map;
};
typedef int(__cdecl *Func_DrawTextEx)(void *dest, int draw_mode2, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
int font, TCHAR *str, int color, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int cnt_flag, TCHAR *normal_str, int kaigyou_musi);
PVOID p_DrawTextEx=NULL;

unordered_map<unsigned __int32, wstring> TranInfo;
unordered_map<wchar_t, wchar_t> Mapinfo;
list<string> rebuildlist;
std::mutex mut;
set<unsigned __int32> tmpset;
void ProcSubStr(unordered_map<unsigned __int32, wstring>& info,string str)
{
	mut.lock();
	CRC32 crc32;
	crc32.Init();
	auto oldString = ctow(str.c_str());
	unsigned __int32 strcrc = crc32.Calc((char*)oldString, wcslen(oldString)*2);
	if (tmpset.find(strcrc) == tmpset.end())
	{
		cout <<endl<< "CRC:0x"<<hex << strcrc << endl;
		tmpset.insert(strcrc);
		cout << "TEXT:" << wtocGBK(oldString)<<endl;
	}
	if (info.find(strcrc) != TranInfo.end())
	{
		//rebuildlist.push_back(info.at(strcrc));
		wstring rebuildStr = info.at(strcrc);
		int idx = 0;
		for (int i=0;i<info.at(strcrc).length();i++)
		{
			wchar_t tag = info.at(strcrc)[i];
			auto res = Mapinfo.find(tag);
			if (res !=Mapinfo.end())
			{
				wchar_t getwch = res->second;
				rebuildStr[i] = getwch;
			}
		}
		string strtmp = string(wtoc(rebuildStr.c_str()));
		if (strtmp.length()==0)
		{
			MessageBoxW(NULL, L"字符串处理失败", L"严重错误", MB_OK | MB_ICONWARNING);
		}
		rebuildlist.push_back(strtmp);
	}
	else
	{
		rebuildlist.push_back(str);
	}
	mut.unlock();
}

int __cdecl HookDrawTextEx(void *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int *a9, int *a10, RECT *a11,
int a12, TCHAR *a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20, int a21, int a22, TCHAR *a23, int a24)
{
	rebuildlist.clear();
	string strStm((char*)a13);
	string token;
	size_t pos1, pos2;
	pos1 = 0;
	const char* spl=R"(\s)";
	pos2 = strStm.find(spl);
	while(string::npos!=pos2){
		ProcSubStr(TranInfo, strStm.substr(pos1, pos2- pos1));
		pos1 = pos2 + strlen(spl);
		pos2 = strStm.find(spl, pos1);
	}
	ProcSubStr(TranInfo, strStm.substr(pos1));
	string rebuild_Str;

	if (!rebuildlist.empty())
	{
		int counter = 0;
		for (auto itr= rebuildlist.begin();itr!= rebuildlist.end();++itr)
		{
			counter++;
			rebuild_Str += *itr;
			if (counter< rebuildlist.size())
			{
				rebuild_Str += "\\s";
				
			}
		}
	}
	
	return ((Func_DrawTextEx)p_DrawTextEx)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, (TCHAR*)rebuild_Str.c_str(), a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24);
}
 


void init()
{
	DWORD badd = (DWORD)GetModuleHandle(NULL);
	p_DrawTextEx = (PVOID)(badd + 0xAF290);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&p_DrawTextEx, HookDrawTextEx);
	if (DetourTransactionCommit() != NO_ERROR)
	{
		MessageBoxW(NULL, L"Hook目标函数失败", L"严重错误", MB_OK | MB_ICONWARNING);
	}

	
}
LPWSTR ctowUTF(char* str)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); //计算长度
	LPWSTR out = new wchar_t[dwMinSize];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, out, dwMinSize);//转换
	return out;
}

void LoadTranslateInfo()
{
	ifstream fin("Script.txt");
	const int LineMax = 0x500;
	char str[LineMax];
	if (fin.is_open())
	{
		cout << "LOAD:" << "Load start." << endl;
		int counter = 0;
		while (fin.getline(str, LineMax))
		{
			auto wtmp = ctowUTF(str);
			wstring wline = wtmp;
			wstring crcval = wline.substr(2, 8);
			wstring wstr = wline.substr(11);
			DWORD crc = wcstoul(crcval.c_str(), NULL, 16);
			TranInfo.insert(pair<DWORD, wstring>(crc, wstr));
			counter++;
		}
		cout << "LOAD:" << counter << " Recoders." << endl;
	}
	else
	{
		MessageBox(0, L"File Open fail!", L"Error", MB_OK);
	}
}

void LoadMapInfo()
{
	fstream file1;
	file1.open("map.dat", ios::binary | ios::in);
	file1.seekg(0, ios::end);
	DWORD szOfFile = file1.tellg();
	file1.seekg(0, ios::beg);
	int totload = szOfFile / sizeof(block);
	auto buffer = new block[totload];
	file1.read((char*)buffer, szOfFile);
	for (unsigned int i=0;i< szOfFile / sizeof(block);i++)
	{
		Mapinfo.insert(pair<wchar_t, wchar_t>(buffer[i].Map, buffer[i].Ori));
	}
	delete[] buffer;
	std::cout << "Load MAP:"<< totload <<" Recoders"<< std::endl;
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
		make_console();
		LoadMapInfo();
		LoadTranslateInfo();
		init();
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