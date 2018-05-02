#include "stdafx.h"
#include "TransText.h"


TransText::TransText()
{
	CurIdx = -1;
	TableLen = 0;
	StorageBuf = nullptr;
	SkipList[0] = 0xa5554085;
}


TransText::~TransText()
{
}

wchar_t * TransText::searchtext(wchar_t * RawStr)
{
	CRC32 crc32;
	crc32.Init();
	int Strlen = wcslen(RawStr) * 2;
	__int32 strcrc = crc32.Calc((char*)RawStr, Strlen);
	
	wchar_t* Ret = nullptr;
	TextStruct* ptr = nullptr;
	for (auto& var:SkipList)
	{
		if (strcrc==var)
		{
			return nullptr;
		}
	}
	std::cout << "CRC:0x" << std::hex << strcrc << std::endl;

	if (StorageBuf!=nullptr)
	{
		ptr = StorageBuf;
	}
	else
	{
		return nullptr;
	}
	if (CurIdx==-1)
	{
		CurIdx = 0;
		Ret =NewSearch(strcrc);
	}
	else
	{
		if (CurIdx>TableLen)
		{
			Ret = NewSearch(strcrc);
		}
		if (CurIdx < 0)
		{
			Ret = NewSearch(strcrc);
		}
		else
		{
			if (ptr[CurIdx].crc32val==strcrc)
			{
				std::cout << "CurIdx Match!" << std::endl;
				Ret = ptr[CurIdx].str;
				CurIdx++;
			}
			else
			{
				Ret = NewSearch(strcrc);
			}
			
		}
		
	}
	return Ret;
}

void TransText::setStorage(TextStruct* Buf)
{
	StorageBuf=Buf;
}

void TransText::setTableLength(__int32 len)
{
	TableLen = len;
}

wchar_t* TransText::NewSearch(__int32 crcval)
{
	wchar_t* ret = nullptr;
	TextStruct* TablePtr = nullptr;
	if (StorageBuf != nullptr)
	{
		TablePtr = StorageBuf;
	}
	else
	{
		return nullptr;
	}

	wchar_t* tmpret1 = nullptr;
	wchar_t* tmpret2 = nullptr;
	int tmpidx1 = -1;
	int tmpidx2 = -1;

	volatile bool flag1 = false;
#pragma omp parallel for shared(flag1)
	for (int i=CurIdx;i<TableLen;i++)
	{
		if (flag1) continue;
		if (TablePtr[i].crc32val==crcval)
		{
			tmpret1 = TablePtr[i].str;
			tmpidx1 = i + 1;
			flag1 = true;
		}
	}
	if (ret==nullptr)
	{
		volatile bool flag2 = false;
#pragma omp parallel for shared(flag2)
		for (int i = CurIdx-1; i >= 0; i--)
		{
			if (flag2) continue;
			if (TablePtr[i].crc32val == crcval)
			{
				tmpret2 = TablePtr[i].str;
				tmpidx2 = i + 1;
				flag2 = true;
			}
		}
	}
	if (tmpidx1==-1)
	{
		CurIdx = tmpidx2;
		ret = tmpret2;
	}
	if (tmpidx2==-1)
	{
		CurIdx = tmpidx1;
		ret = tmpret1;
	}
	if (tmpidx1!=-1&&tmpidx2!=-1)
	{
		auto desIdx1 = abs(CurIdx - tmpidx1);
		auto desIdx2 = abs(CurIdx - tmpidx2);
		if (desIdx1<desIdx2)
		{
			std::cout << "choose Forward search result!" << std::endl;
			CurIdx = tmpidx1;
			ret = tmpret1;
		}
		else
		{
			std::cout << "choose Reverse search result!"<< std::endl;
			CurIdx = tmpidx2;
			ret = tmpret2;
		}
	}
	return ret;
}