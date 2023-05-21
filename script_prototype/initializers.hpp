#ifndef _initial_
#define _initial_

#include "pch.h"

namespace init
{
	struct initializer_list_s {
		std::shared_ptr<expr::expression_token> values;
		unsigned __int16 numElements;
	};

	void SetVariableInitializer(Variable& target, const std::string& expression);
	bool IsInitializerList(std::string::const_iterator& it, std::string::const_iterator end);
	void EvaluateInitializerList(Variable* var, const std::string& expression);
	void TokenizeInitializerLists(std::string expr, std::list<std::string>& tokens);

	Substr_s FindMatchingCurlyBracket(const std::string_view& expr);
}

#endif