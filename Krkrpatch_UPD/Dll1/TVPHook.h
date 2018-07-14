#pragma once
#include "tp_stub.h"
/*V2Link*/
typedef HRESULT(__stdcall *PfuncV2Link)(iTVPFunctionExporter *);
/*CreateStream*/
typedef tTJSBinaryStream* (__fastcall *PfuncCreateStream)(const ttstr &, tjs_uint32);
/*HOOK MultiByteToWideChar*/
typedef int(WINAPI *PfuncMultiByteToWideChar)(
	_In_      UINT   CodePage,
	_In_      DWORD  dwFlags,
	_In_      LPCSTR lpMultiByteStr,
	_In_      int    cbMultiByte,
	_Out_opt_ LPWSTR lpWideCharStr,
	_In_      int    cchWideChar);
/*HOOK LoadLibraryW*/
typedef HMODULE(WINAPI *PfuncLoadLibraryW)(
	_In_ LPCTSTR lpFileName
	);
class TVPHook
{
public:
	static TVPHook* handle;

	TVPHook();
	~TVPHook();
	bool init();
	bool uninit();
	//void initKrkrHook(LPCWSTR lpFileName, HMODULE Module);
	IStream* CreateLocalStream(LPCWSTR lpFileName);
	PVOID g_POldV2Link;
	PVOID g_POldCreateStream;
	PVOID g_pOldMultiByteToWideChar;
	PVOID g_pOldLoadLibraryW;
	iTVPFunctionExporter* TVPFunctionExporter;
	bool inited;
};

TVPHook* GetobjTVPhook();

