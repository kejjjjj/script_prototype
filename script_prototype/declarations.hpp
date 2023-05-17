#ifndef codedecls
#define codedecls

#include "pch.h"

namespace decl
{
	//struct var_tree_declr
	//{
	//	std::optional<int> 
	//};

	void EvaluateDeclaration(const std::string_view& type, std::string::iterator& it);
	int EvaluateArrayInitialSize(const std::string& expression);

	void EvaluateDeclarationOperators(std::string::iterator& it, std::string::iterator& end, std::list<var_declr_data>& datalist);
	std::string ParseArrayExpression(std::string::iterator& it, std::string::iterator& end);

	void RearrangeModifierOrder(std::list<std::optional<int>>::iterator& it, std::list<std::optional<int>>& modifiers);
	void PopulateVariableTree(Variable* parent, std::list<std::optional<int>> children);


}

#endif