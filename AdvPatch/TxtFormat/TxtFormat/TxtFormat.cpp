// TxtFormat.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
using namespace std;

LPWSTR ctowUTF(char* str)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); //计算长度
	LPWSTR out = new wchar_t[dwMinSize];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, out, dwMinSize);//转换
	return out;
}
char* wtocUTF(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_UTF8, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度
	char *out = new char[dwMinSize];
	WideCharToMultiByte(CP_UTF8, NULL, str, -1, out, dwMinSize, NULL, FALSE);//转换
	return out;
}



int main(int argc,char* argv[])
{
	if (argc!=2)
	{
		cout << "Usage:" << argv[0] << " UTF-8 TxtFile" << endl;
	}
	vector<DWORD> crclist;
	ifstream fin(argv[1]);
	const int LineMax = 0x400;
	char str[LineMax];
	vector<wstring>rawlist;
	vector<wstring>newlist;
	if (fin.is_open())
	{
		int counter = 0;
		while (fin.getline(str, LineMax))
		{
			auto wtmp = ctowUTF(str);
			wstring wline = wtmp;
			if (wline[1]== (wchar_t)L'/' && wline[1] == (wchar_t)L'/')
			{
				wline = wline.substr(2);
				rawlist.push_back(wline);
			}
			else
			{
				newlist.push_back(wline);
			}
		}
		fin.close();
	}
	else
	{
		fin.close();
		cout << "Open File Fail!: " << argv[1]<<endl;
		return -1;
	}
	cout << "Load New Texts from: " << argv[1] << endl;
	if (rawlist.size()!=newlist.size())
	{
		cout << "The vector size is not equal!: " << endl;
		return -1;
	}
	cout << "Computing CRC32..." << endl;
	size_t index=0;
	CRC32 crc;
	for (index;index<rawlist.size();++index)
	{
		crc.Init();
		DWORD crcval = crc.Calc((char*)rawlist.at(index).c_str(), rawlist.at(index).length()*2);
		crclist.push_back(crcval);
	}
	cout << "Output files" << endl;
	string namefile = argv[1];
	namefile += ".out";
	ofstream fout(namefile);
	for (int i=0;i<crclist.size();i++)
	{
		fout << "0x" << hex << setw(8) << setfill('0') << crclist[i] << "|" << wtocUTF(newlist[i].c_str()) << endl;
	}

    return 0;
}

