#pragma once
#pragma pack(1)
struct TextStruct
{
	__int32 crc32val;
	wchar_t str[126];
};
#pragma pack(1)
struct head
{
	char flag[8];
	__int32 Length;
	__int32 Zero;
};
#pragma pack()
class TransText
{
private:
	__int32 CurIdx;
	__int32 TableLen;
	TextStruct* StorageBuf;
	wchar_t* NewSearch(__int32 crcval);
	__int32 SkipList[128];
public:
	TransText();
	~TransText();
	wchar_t* searchtext(wchar_t* RawStr);
	void setStorage(TextStruct* Buf);
	void setTableLength(__int32 len);
};

