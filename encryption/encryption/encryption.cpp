// encryption.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
typedef unsigned char byte;
typedef unsigned short word;
unsigned char* buf;
unsigned char* data;
typedef unsigned int ulong;
using namespace std;
string fileext = ".dec";
byte key = 0;

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		cout << "Usage:" << argv[0] << " ScriptFile" << endl;
		return 0;
	}
	if (argc >= 3)
	{
		cout << "Incorrect parameter!" << endl;
		return 0;
	}//Parameter Check
	string OutFile;
	string TMPName;
	TMPName = argv[1];
	int pos1 = TMPName.find(fileext);
	cout << "Input File: " << TMPName << endl;
	if (pos1 != TMPName.npos)
	{
		TMPName = TMPName.erase(pos1, fileext.length());
		OutFile = TMPName + ".enc";
	}
	else
	{
		cout << "Incorrect Input File!" << endl;
		return -1;
	}
	fstream TFile2(OutFile, ios::binary | ios::out | ios::ate);
	fstream TFile(argv[1], ios::binary | ios::in | ios::out);
	if (TFile.fail())
	{
		cout << "Open Fail:File " << argv[1] << endl;
		return -1;
	}
	cout << "Target File:" << argv[1] << " Open Success!" << endl;//Open File
	int Filesize;
	TFile.seekg(0, ios::end);
	Filesize = TFile.tellg();
	cout << "File Size:" << Filesize << "Byte" << endl;
	CHAR* TMPBlock;
	TMPBlock = new char[Filesize];
	TFile.seekg(0x0, ios::beg);
	try
	{
		TFile.read(TMPBlock, Filesize);
	}
	catch (exception ex)
	{
		cout << "Read operation Failed!";
		return -1;
	}
	cout << "File read success!" << endl;
	cout << "Decryption Start!" << endl;
	for (int i = 0; i < Filesize; i++)
	{
		
		TMPBlock[i] ^= key;
		byte tmp = TMPBlock[i];
		tmp += 0x12;
		key += tmp;
	}
	cout << "Decryption Finish!" << endl;
	TFile2.write(TMPBlock, Filesize);
	cout << "Write File:" << OutFile << endl;
	delete[] TMPBlock;
	return 0;
}