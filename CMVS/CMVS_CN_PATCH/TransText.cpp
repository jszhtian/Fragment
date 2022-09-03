#include "TransText.h"

TransText::TransText()
{
    //fTextWrite.open(L"Project\\news.txt", std::ios::out);
}

TransText::~TransText()
{
    if (fTextRead.is_open())
        fTextRead.close();
    //if (fTextWrite.is_open())
    //    fTextWrite.close();
}
uint32_t GetHash32(const wchar_t* oldText, unsigned int offset)
{
    
    uint32_t hash=0;
    if (oldText == nullptr)
        return hash;
    MurmurHash3_x86_32(oldText, wcslen(oldText) * 2, offset, &hash);
    return hash;
}

std::string TransText::Query(wchar_t* oldText, unsigned int offset)
{
    if(oldText == NULL)
        return std::string();
    uint32_t hash = 0;
    hash = GetHash32(oldText, offset);
    auto it = container.find(hash);
    if (it != container.end())
    {
        std::wstring targetStr = it->second;
        DWORD dwMinSize;
        dwMinSize = WideCharToMultiByte(936, NULL, targetStr.c_str(), -1, NULL, 0, NULL, FALSE); //计算长度
        char* out = new char[dwMinSize];
        WideCharToMultiByte(936, NULL, targetStr.c_str(), -1, out, dwMinSize, NULL, FALSE);//转换
        std::string outStr(out);
        delete[] out;
        return outStr;
    }
    else
    {
        //writeToFile(oldText, offset);
    }
    return std::string();
}

void TransText::LoadFromFile()
{
    fTextRead.open(L"Project\\trans.txt", std::ios::in);
    const int LineMax = 0x100;
    char str[LineMax];

    while (fTextRead.getline(str, LineMax))
    {
        DWORD dwMinSize;
        dwMinSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); //计算长度
        wchar_t s[0x100];
        MultiByteToWideChar(CP_UTF8, 0, str, -1, s, dwMinSize);//转换

        uint32_t hash;
#if(USE_PRE_CALC_HASH)
        std::wstringstream sstr(s);
        std::wstring arr_s[3];
        int index = 0;
        while (sstr.good())
        {
            std::wstring substr;
            getline(sstr, substr, L'|');
            arr_s[index] = substr;
            index++;
            
        }
        hash = std::stoi(arr_s[0], nullptr, 16);
#else
        std::wstringstream sstr(s);
        std::wstring arr_s[3];
        int index = 0;
        while (sstr.good())
        {
            std::wstring substr;
            getline(sstr, substr, L'|');
            arr_s[index] = substr;
            index++;
        }
        hash = GetHash32(arr_s[1].c_str(), std::stoi(arr_s[0], nullptr, 16));
#endif
        std::wcout << arr_s[0] << std::endl;
        std::cout << "Load:" << std::hex << hash << std::endl;
        container.insert(std::make_pair(hash, arr_s[2]));
    }
    std::cout << "Load " << container.size() << " translations!" << std::endl;
    fTextRead.close();
}

//void TransText::writeToFile(std::wstring oldText, unsigned int offset)
//{
//}
