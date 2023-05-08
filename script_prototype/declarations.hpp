#ifndef codedecls
#define codedecls

#include "pch.h"

namespace decl
{
	
	void EvaluateDeclaration(const std::string_view& type, std::string::iterator& it);
	int EvaluateArrayInitialSize(const std::string& expression);
}

#endif