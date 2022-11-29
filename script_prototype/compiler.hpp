#pragma once

#ifndef scompiler
#define scrompiler

#include "pch.h"

class Compiler
{
public:
	Compiler(std::string path);
	
	~Compiler();
	bool Compile();

private:
	std::fstream f;
};

#endif
