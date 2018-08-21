// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include "crc32.h"
#include <sstream>
#include "detours.h"
#include <iomanip>
#include <mutex>
#include <list>
using namespace std;
ofstream flog("out.log");
typedef int(__cdecl *Func_DrawTextEx)(void *dest, int draw_mode2, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
int font, TCHAR *str, int color, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int cnt_flag, TCHAR *normal_str, int kaigyou_musi);
PVOID p_DrawTextEx=NULL;
unordered_map<__int32, string> TranInfo;
list<string> rebuildlist;
std::mutex mut;
void ProcSubStr(unordered_map<__int32, string>& info,string str)
{
	mut.lock();
	CRC32 crc32;
	crc32.Init();
	int Strlen = str.length();
	__int32 strcrc = crc32.Calc(str.c_str(), Strlen);
	if (info.find(strcrc) == TranInfo.end())
	{
		flog << "0x" << hex << setw(8) << setfill('0') << strcrc << "|" << str.c_str() << endl;
		info.insert(pair<__int32, string>(strcrc, str));
	}
	mut.unlock();
}

int __cdecl HookDrawTextEx(void *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int *a9, int *a10, RECT *a11,
int a12, TCHAR *a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20, int a21, int a22, TCHAR *a23, int a24)
{
	//flog << (char*)a13 << endl;
	string strStm((char*)a13);
	//flog << strStm << endl;
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
	
	return ((Func_DrawTextEx)p_DrawTextEx)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24);
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
void loadlist()
{
	char teststr[] = { 0x81,0x41,0x81,0x41,0x81,0x41,0x0 };
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
		init();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

