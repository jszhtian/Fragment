#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <tuple>
#include <windows.h>
#include "MurmurHash3.h"

#define USE_PRE_CALC_HASH		(0)

class TransText
{
public:
	TransText();
	~TransText();
	std::string Query(wchar_t* oldText, unsigned int offset);
	void LoadFromFile();
	//void writeToFile(std::wstring oldText, unsigned int offset);
private:
	std::ifstream fTextRead;
	//std::ofstream fTextWrite;
	std::unordered_map<uint32_t, std::wstring> container;
};

