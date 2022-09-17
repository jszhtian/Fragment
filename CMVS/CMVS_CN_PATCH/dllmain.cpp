// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include "TransText.h"

DWORD str_off = 0;
CHAR CopyStr[0x1000] = { 0 };
CHAR* FileName = nullptr;
TransText transtext;
wchar_t wout[0x100];

bool isAllAscii(string s)
{
    for (unsigned char ch : s)
    {
        if (ch >> 7) // whether the first bit is 1
        {
            return false;
        }
    }
    return true;
}


//char trans[] = "友達だと思っていたやつが親に言われて態度を変え、面倒くさいと距離を置き、あげくには教師にまで特別扱いされれば誰だってトラ";

void __stdcall ProcessPushStr(UINT str, UINT off)
{
    str_off = off;
    if (strlen((char*)(str + off)) == 0)
        return;
    if (isAllAscii((char*)(str + off)))
        return;

    //cout << "0x" << hex << off << "|"<< strlen((char*)(str + off)) << "|" << wtoc(ctow((char*)(str + off), 932), 936) << endl;
    
#if(0)
    //TODO: Add Func Map
    str_off = off;
    cout << "0x" << hex << off << "|" << wtoc(ctow((char*)(str + off), 932), 936) << endl;
    ///*
    if (off == 0x980)
    {
        lstrcpyA(CopyStr, "- 心１ -");
        str_off = (UINT)CopyStr - str;
    }
    else if (off == 0x8E4)
    {
        lstrcpyA(CopyStr, "中文测试１２３ＡＢＣａｂｃ");
        str_off = (UINT)CopyStr - str;
    }
    else if (off == 0x8F3)
    {
        lstrcpyA(CopyStr, "中文测试");
        str_off = (UINT)CopyStr - str;
    }
    else if (off == 0x16C)
    {
        lstrcpyA(CopyStr, "库洛的时钟");
        str_off = (UINT)CopyStr - str;
    }
   // */
#endif
#if(0)
    if (off == 0x980)
    {
        lstrcpyA(CopyStr, "- 心１ -");
        str_off = (UINT)CopyStr - str;
    }
    else if (off == 0x8E4)
    {
        lstrcpyA(CopyStr, "中文测试１２３ＡＢＣａｂｃ");
        str_off = (UINT)CopyStr - str;
    }
    else if (off == 0x8F3)
    {
        lstrcpyA(CopyStr, "中文测试");
        str_off = (UINT)CopyStr - str;
    }
    else if (off == 0x16C)
    {
        lstrcpyA(CopyStr, "库洛的时钟");
        str_off = (UINT)CopyStr - str;
    }
    else
    {
        strcpy(CopyStr, trans);
        str_off = (UINT)CopyStr - str;
    }
   
#endif

#if(1)
    auto newText = transtext.Query((char*)(str + off), off);

    if (newText.length() != 0)
    {
        if (newText.length() > 0x78)
        {
            cout << "OOM: 0x" << hex << off << " | " << wtoc(ctow((char*)(str + off), 932), 936) << endl;
        }
        else
        {
            lstrcpyA(CopyStr, newText.c_str());
            str_off = (UINT)CopyStr - str;
            //cout << "Replace: 0x" << hex << off << " | " << wtoc(ctow((char*)(str + off), 932), 936) << endl;
        }
        
    }
    else
    {
        cout << "Skip 0x" << hex << str_off << " | " << wtoc(ctow((char*)(str + off), 932), 936) << endl;
    }
#endif
    
    
    return;
}

PVOID HookAddr_pPorcessPushStr = (PVOID)0x458282;
PVOID JmpOut_pPorcessPushStr = (PVOID)0x458289;
__declspec(naked)void ASM_ProcessPushStr()
{
    __asm
    {
        mov ecx, dword ptr ds : [esi + edx * 4 + 0x3AFC] ;
        pushad;
        pushfd;
        push eax;
        push ecx;
        call ProcessPushStr;
        popfd;
        popad;
        mov eax, str_off;
        jmp JmpOut_pPorcessPushStr
    }
}


PVOID HookAddr_pPorcessPushStr2 = (PVOID)0x45E570;
PVOID JmpOut_pPorcessPushStr2 = (PVOID)0x45E577;
__declspec(naked)void ASM_ProcessPushStr2()
{
    __asm
    {
        mov eax, dword ptr ds : [ecx + eax * 4 + 0x3AFC] ;
        pushad;
        pushfd;
        push edx;
        push eax;
        call ProcessPushStr;
        popfd;
        popad;
        mov edx, str_off;
        jmp JmpOut_pPorcessPushStr2
    }
}

PBYTE NTAPI LoadImageRedirect(LPCSTR lpFileName, LPBYTE Buffer)
{
    //TODO: FileIO
    PBYTE    ImageBuffer;
    SIZE_T   Size;

    ImageBuffer = NULL;
    Size = 0;

    static wchar_t fnmbuffer[256];
    memset(fnmbuffer, 0, sizeof(wchar_t) * 256);
    static wchar_t filePath[256];
    memset(filePath, 0, sizeof(wchar_t) * 256);
    if (lpFileName)
    {
        size_t nu = strlen(lpFileName);
        size_t n = (size_t)MultiByteToWideChar(CP_ACP, 0, lpFileName, int(nu), NULL, 0);
        memset(fnmbuffer, 0, sizeof(wchar_t) * 256);
        MultiByteToWideChar(CP_ACP, 0, lpFileName, int(nu), fnmbuffer, int(n));
    }
    wcscpy(filePath, L"project\\");
    wcscat(filePath, fnmbuffer);
    wcscat(filePath, L".bmp");
    HANDLE pFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (pFile == INVALID_HANDLE_VALUE)
    {
        cout << "SKIP_IMG:" << lpFileName << endl;
        return Buffer;
    }
    auto fileSize = GetFileSize(pFile, NULL);
    ImageBuffer = new BYTE[fileSize];
    ReadFile(pFile, ImageBuffer, fileSize, &Size, NULL);
    memcpy(Buffer, ImageBuffer + 0x36, Size - 0x36);
    CloseHandle(pFile);
    delete[] ImageBuffer;
    cout << "REPLACE_IMG:" << lpFileName << endl;
    return Buffer;
}

LPVOID DetouredLoadImage_Start = (LPVOID)0x0050BE46;

__declspec(naked) VOID DetouredLoadImage()
{
    __asm
    {
        mov eax, dword ptr[esp + 0x15C]
        pushad
        push ebx
        push eax
        call LoadImageRedirect
        popad

        ; original code here
        pop edi
        pop esi
        pop ebp
        mov eax, ebx
        pop ebx
        add esp, 0x48
        retn 0xC
    }
}

void InlinePatch()
{
    DetourTransactionBegin();

    DetourAttach(&HookAddr_pPorcessPushStr, ASM_ProcessPushStr);
    DetourAttach(&HookAddr_pPorcessPushStr2, ASM_ProcessPushStr2);

    DetourAttach(&DetouredLoadImage_Start, DetouredLoadImage);
    if (DetourTransactionCommit() != NOERROR)
    {
        MessageBox(NULL, L"Patch hook error", L"Patch", MB_OK | MB_ICONERROR);
        ExitProcess(-1);
    }
}

PVOID g_pOldCreateFontA = CreateFontA;
typedef HFONT (WINAPI* PfuncCreateFontA)(int nHeight,
    int nWidth,
    int nEscapement,
    int nOrientation,
    int fnWeight,
    DWORD fdwltalic,
    DWORD fdwUnderline,
    DWORD fdwStrikeOut,
    DWORD fdwCharSet,
    DWORD fdwOutputPrecision,
    DWORD fdwClipPrecision,
    DWORD fdwQuality,
    DWORD fdwPitchAndFamily,
    LPCTSTR lpszFace);
HFONT WINAPI HookCreateFontA(int nHeight,
    int nWidth,
    int nEscapement,
    int nOrientation,
    int fnWeight,
    DWORD fdwltalic,
    DWORD fdwUnderline,
    DWORD fdwStrikeOut,
    DWORD fdwCharSet,
    DWORD fdwOutputPrecision,
    DWORD fdwClipPrecision,
    DWORD fdwQuality,
    DWORD fdwPitchAndFamily,
    LPCTSTR lpszFace)
{
    return CreateFontW(nHeight,
        nWidth,
        nEscapement,
        nOrientation,
        fnWeight,
        fdwltalic,
        fdwUnderline,
        fdwStrikeOut,
        GB2312_CHARSET,
        fdwOutputPrecision,
        fdwClipPrecision,
        fdwQuality,
        fdwPitchAndFamily,
        L"SimHei");
}

PVOID g_pOldSetWindowTextA = SetWindowTextA;
typedef bool (WINAPI* PfuncSetWindowTextA)(HWND hWnd, LPCSTR lpString);
bool WINAPI HookSetWindowTextA(HWND hw, LPCSTR lpString)
{
    //cout << lpString << endl;
    for (int i = 0; i < lstrlenA(lpString);)
    {
        UINT c1 = lpString[i] & 0xFF;
        if (c1 == 0x81 && (UINT)lpString[i + 1] == 0x40)
        {
            memcpy((void*)(lpString + i), "\xA1\xA1", 2);
            i += 2;
        }
        else
            i++;
    }
    return ((PfuncSetWindowTextA)g_pOldSetWindowTextA)(hw, lpString);
}

void Init()
{
    DetourTransactionBegin();
    DetourAttach(&g_pOldCreateFontA, HookCreateFontA);
    DetourAttach(&g_pOldSetWindowTextA, HookSetWindowTextA);
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
        make_console();
        InlinePatch();
        Init();
        transtext.LoadFromFile();
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
