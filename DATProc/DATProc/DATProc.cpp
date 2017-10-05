// DATProc.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#define HEAD_STRING "$TEXT_LIST__"
typedef unsigned char byte;
typedef unsigned int uint;

long get_file_size(FILE* f) {
	long size_of_file;
	fseek(f, 0, SEEK_END);
	size_of_file = ftell(f);
	fseek(f, 0, SEEK_SET);
	return size_of_file;
}
#define _read_uint(pt,uv) \
		uv = *(uint*)pt; \
		pt += sizeof(uint) 

char* _read_str(byte** pt)
{
	char* ptx;
	ptx = (char*)*pt;
	*pt += strlen(ptx) + 1;

	return ptx;
}
int CovtShiftJISToGB(const char* JISStr, char* GBBuf, int nSize)
{
	static wchar_t wbuf[2048];
	int nLen;

	nLen = strlen(JISStr) + 1;
	if (wbuf == NULL) return 0;
	nLen = MultiByteToWideChar(932, 0, JISStr,
		nLen, wbuf, nLen);
	if (nLen > 0)
		nLen = WideCharToMultiByte(936, 0, wbuf,
			nLen, GBBuf, nSize, NULL, 0);
	return nLen;
}
int CovtGbkToShiftJIS(const char* JISStr, char* GBBuf, int nSize)
{
	static wchar_t wbuf[2048];
	int nLen;

	nLen = strlen(JISStr) + 1;
	if (wbuf == NULL) return 0;
	nLen = MultiByteToWideChar(936, 0, JISStr,
		nLen, wbuf, nLen);
	if (nLen > 0)
		nLen = WideCharToMultiByte(932, 0, wbuf,
			nLen, GBBuf, nSize, NULL, 0);
	return nLen;
}

void dec_char(byte* c)
{
	byte* pt;
	uint strcount;
	uint strid;
	char new_line_byte = 0xA;
	static char conv_char[2048];

	uint curid;
	char* cursc;
	FILE* f;

	pt = c;

	pt += sizeof(HEAD_STRING) - 1;

	_read_uint(pt, strcount);
	f = fopen("sens.txt", "w+");

	printf("strcount:%u\n", strcount);

	for (strid = 0; strid < strcount; strid++)
	{
		_read_uint(pt, curid);
		cursc = _read_str(&pt);

		CovtShiftJISToGB(cursc, conv_char, sizeof(conv_char));
		fwrite(conv_char, strlen(conv_char), 1, f);
		fwrite(&new_line_byte, 1, 1, f);
	}
	fclose(f);
	printf("success\n");
	getchar();
}
int main()
{
	byte * data;
	long size_of_file;

	FILE* f;


	f = fopen("TEXT.DAT", "rb");

	if (!f)
	{
		printf("open file error\n");
		return -1;
	}

	size_of_file = get_file_size(f);

	data = (byte*)malloc(size_of_file);

	fread(data, size_of_file, 1, f);

	fclose(f);

	if (memcmp(data, HEAD_STRING,12) == 0)
	{
		dec_char(data);
	}
	free(data);
	return 1;
}
