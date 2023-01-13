#pragma once

#ifndef scompiler
#define scrompiler

#include "pch.h"

class Script : protected Runtime
{
public:
	Script(const std::string& path);
	~Script();

	bool Compile();

	std::string rtScript;

private:
	std::fstream f;
	bool compiled;
	std::string compiled_file;
};

#endif
