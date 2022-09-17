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
#include <mutex>
#include <queue>

#define BUFFER_SIZE				5
#define FAST_SEARCH_DEEPTH		10
//FAKE hasher for existing murmurhash uint32_t
template <class T>
class MyHash;
template<>
class MyHash<uint32_t>
{
public:
	std::size_t operator()(uint32_t const& s) const
	{
		return static_cast<std::size_t>(s);
	}
};

class TransText
{
public:
	TransText();
	~TransText();
	std::string Query(char* oldText, unsigned int offset);
	void LoadFromFile();
	std::string search_buffer(std::string);
	void insert_buffer(std::string, std::string);
	std::string fast_search(std::string);
private:
	std::ifstream fTextRead;
	std::unordered_multimap <uint32_t, uint32_t,MyHash<uint32_t>> indexMap;
	std::vector<std::string> text_raw;
	std::vector<std::string> text_trans;
	size_t buf_index;
	size_t current_index;
	std::string next_raw;
	std::string next_trans;
	std::vector<std::string> buf_raw = std::vector(BUFFER_SIZE, std::string(""));
	std::vector<std::string> buf_trans = std::vector(BUFFER_SIZE, std::string(""));
};

