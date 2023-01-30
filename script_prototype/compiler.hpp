#pragma once

#ifndef scompiler
#define scrompiler

#include "pch.h"

inline std::fstream f;

class Script : protected Runtime
{
public:
	Script(const std::string& path);
	~Script();

	bool Compile();

	std::string rtScript;

private:
	
	bool compiled = false;
	std::string compiled_file;
};

#endif
