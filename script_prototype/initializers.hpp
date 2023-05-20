#ifndef _initial_
#define _initial_

#include "pch.h"

namespace init
{
	void SetVariableInitializer(Variable& target, const std::string& expression);
	bool IsInitializerList(std::string::const_iterator& it, std::string::const_iterator end);
	//void EvaluateEntireInitializerList(std::list<Variable&>& vars, const std::string& expression);
}

#endif