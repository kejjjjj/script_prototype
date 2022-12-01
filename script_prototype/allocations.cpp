
#include "pch.h"

void Memory_c::PushToStack(Variable var)
{
	if (stack.size() + 1 >= stack.max_size()) {
		CompilerError("Script stack overflow");
		return;
	}
	stack.push_back(var);
}
void Memory_c::PopFromStack(const size_t idx)
{
	
	stack.erase(stack.begin()+idx);
}