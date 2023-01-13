#pragma once

#ifndef __allocate
#define __allocate

#include "pch.h"

class Memory_c
{
public:
	
	void PushToStack(Variable var);
	void PopFromStack(const size_t idx);

	std::vector<Variable> stack; //current variables on the stack
	int32_t stacklevel = 0; 

private:
};

inline Memory_c scriptStack;

#endif