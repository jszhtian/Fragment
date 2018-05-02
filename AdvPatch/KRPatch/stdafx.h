// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>
#include <iostream>  
#include "detours.h"
#include <filesystem>
#include <string>
#include <cstring>
#include <exception>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <atlstr.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "crc32.h"
#include <iomanip>
#include <mutex>
#include "zlib.h"
#pragma comment(lib, "detours.lib")
#pragma comment(lib,"zlib.lib")



// TODO:  在此处引用程序需要的其他头文件
