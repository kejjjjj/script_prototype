
#pragma once

#ifndef PCH_H
#define PCH_H

#define NOMINMAX

#define ENABLE_COMPILER_LOGGING 1

#if ENABLE_COMPILER_LOGGING
#define LOG(x) std::cout << x
#else
#define LOG(x)
#endif

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <filesystem>
#include <direct.h>
#include <array>
#include <list>
#include <ranges>
#include <functional>
#include <unordered_map>
#include <any>
#include <numeric>

#define MAKE_LOWORD(dword, word) ((dword) = ((dword) & 0xFFFF0000) | (WORD)(word))
#define MAKE_HIWORD(dword, word) ((dword) = ((dword) & 0x0000FFFF) | ((DWORD)(word) << 16))


#endif