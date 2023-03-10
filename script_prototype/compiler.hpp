#pragma once

#ifndef scompiler
#define scrompiler

#include "pch.h"

inline std::string f_str;
inline Syntax syntax;
class Script : protected Runtime
{
public:
	Script(const std::string& path);
	~Script() = default;

	bool Compile();

	std::string rtScript;

private:
	std::fstream f;
};

#endif
