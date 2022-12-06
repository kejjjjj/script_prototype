#pragma once

#ifndef runtime_
#define runtime_

#include "pch.h"

class Runtime
{
public:

	Runtime();
	~Runtime();

	bool InitializeRuntime(const std::string& path);

private:
	std::fstream f;
};


#endif