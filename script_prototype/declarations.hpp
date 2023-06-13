#ifndef codedecls
#define codedecls

#include "pch.h"

namespace decl
{
	void EvaluateDeclaration(const std::string_view& type, std::string::iterator& it);
	int EvaluateArrayInitialSize(const std::string& expression);

	void EvaluateDeclarationOperators(std::string::iterator& it, std::string::iterator& end, std::list<var_declr_data>& datalist);
	std::string ParseArrayExpression(std::string::iterator& it, std::string::iterator& end);

	void PopulateVariableTree(Variable* parent, std::list<std::optional<int>> children);


}

#endif