#include "TransText.h"
static std::mutex mtx;
TransText::TransText()
{
    next_raw = "";
    next_trans = "";
    current_index = 0;
    buf_index = 0;
}

TransText::~TransText()
{
    if (fTextRead.is_open())
        fTextRead.close();
}
uint32_t GetHash32(const char* oldText, unsigned int offset)
{
    
    uint32_t hash=0;
    if (oldText == nullptr)
        return hash;
    MurmurHash3_x86_32(oldText, strlen(oldText), offset, &hash);
    return hash;
}

std::string TransText::Query(char* oldText, unsigned int offset)
{
    std::lock_guard<std::mutex> lck(mtx);
    if(oldText == NULL)
        return std::string();
    auto result = search_buffer(oldText);
    
    if (next_raw == oldText)
    {
        //std::cout << "CASE_NEXT" << std::endl;
        auto last_raw = oldText;
        auto last_trans = next_trans;
        insert_buffer(last_raw, last_trans);
        current_index ++;
        next_raw = text_raw[current_index];
        next_trans = text_trans[current_index];
        return last_trans;
    }

    if (result.length() != 0)
    {
        //std::cout << "CASE_BUFFER" << std::endl;
        return result;
    }

    auto result2 = fast_search(oldText);
    if (result2.length() != 0)
    {
        //std::cout << "CASE_FAST" << std::endl;
        insert_buffer(oldText, result2);
        current_index++;
        next_raw = text_raw[current_index];
        next_trans = text_trans[current_index];
        return result2;
    }

    uint32_t hash = 0;
    hash = GetHash32(oldText, offset);
    auto pos = indexMap.equal_range(hash);
    if ( (pos.first != indexMap.end()) && (pos.second != indexMap.end()))
    {
        if (std::distance(pos.first, pos.second) == 1)
        {
            //std::cout << "CASE_SCH_0" << std::endl;
            current_index = pos.first->second;
            auto last_raw = oldText;
            auto last_trans = text_trans[current_index];
            insert_buffer(last_raw, last_trans);
            current_index++;
            next_raw = text_raw[current_index];
            next_trans = text_trans[current_index];
            return last_trans;
        }
        else
        {
            for (auto it = pos.first; it != pos.second; ++it)
            {
                if (text_raw[it->second] == oldText)
                {
                    //std::cout << "CASE_SCH_1" << std::endl;
                    current_index = it->second;
                    auto last_raw = oldText;
                    auto last_trans = text_trans[current_index];
                    insert_buffer(last_raw, last_trans);
                    current_index++;
                    next_raw = text_raw[current_index];
                    next_trans = text_trans[current_index];
                    return last_trans;
                }
            }
            return std::string();
        }
        
    }
     else
    {
        
    }
    return std::string();
}

void TransText::LoadFromFile()
{
    fTextRead.open(L"Project\\trans.txt");
    std::string str;
    while (std::getline(fTextRead,str))
    {
        wchar_t s[0x100];
        char raw[0x100];
        char trans[0x100];

        DWORD dwMinSize_cw;
        dwMinSize_cw = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0); //计算长度
        
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, s, dwMinSize_cw);//转换
        uint32_t hash;

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

        
        DWORD dwMinSize_wc932;
        dwMinSize_wc932 = WideCharToMultiByte(932, NULL, arr_s[1].c_str(), -1, NULL, 0, NULL, FALSE); //计算长度
        WideCharToMultiByte(932, NULL, arr_s[1].c_str(), -1, raw, dwMinSize_wc932, NULL, FALSE);//转换

        hash = GetHash32(raw, std::stoi(arr_s[0], nullptr, 16));
        int vec_idx = text_raw.size();
        text_raw.emplace_back(std::string(raw));

        //std::wcout << arr_s[0] << std::endl;
        //std::cout << "Load:" << std::hex << hash << std::endl;
        DWORD dwMinSize_wc936;
        dwMinSize_wc936 = WideCharToMultiByte(936, NULL, arr_s[2].c_str(), -1, NULL, 0, NULL, FALSE); //计算长度
        WideCharToMultiByte(936, NULL, arr_s[2].c_str(), -1, trans, dwMinSize_wc936, NULL, FALSE);//转换
        text_trans.emplace_back(trans);
        indexMap.emplace(hash, vec_idx);
    }
    
    std::cout << "Load " << text_trans.size() << " translations!" << std::endl;
    fTextRead.close();
}

std::string TransText::search_buffer(std::string s)
{
    for (int i = 0; i< buf_raw.size(); ++i)
    {
        if (buf_raw[i] == s)
        {
            return buf_trans[i];
        } 
    }
    return std::string();
}

void TransText::insert_buffer(std::string raw, std::string trans)
{
    if (buf_index < BUFFER_SIZE)
    {
        buf_raw[buf_index] = raw;
        buf_trans[buf_index] = trans;
    }
    else
    {
        buf_index = 0;
        buf_raw[buf_index] = raw;
        buf_trans[buf_index] = trans;
    }
    buf_index++;
}

std::string TransText::fast_search(std::string raw)
{
    bool low_boundary = false;
    bool high_boundary = false;
    if (current_index < FAST_SEARCH_DEEPTH)
    {
        low_boundary = true;
    }
    if (current_index + FAST_SEARCH_DEEPTH > text_raw.size())
    {
        high_boundary = true;
    }
    size_t lower = 0;
    size_t upper = 0;
    if (low_boundary)
    {
        lower = 0;
    }
    else
    {
        lower = current_index - FAST_SEARCH_DEEPTH;
    }
    if (high_boundary)
    {
        upper = text_raw.size();
    }
    else
    {
        upper = current_index + FAST_SEARCH_DEEPTH;
    }
    for (size_t i = lower; i < upper; ++i)
    {
        if (text_raw[i] == raw)
        {
            current_index = i;
            return text_trans[i];
        }
    }
    return std::string();
}

//void TransText::writeToFile(std::wstring oldText, unsigned int offset)
//{
//}
