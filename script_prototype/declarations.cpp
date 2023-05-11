#include "pch.h"

void decl::EvaluateDeclaration(const std::string_view& type, std::string::iterator& it_)
{

	if (!srules.typename_allowed) {
		throw std::exception("type name is not allowed");
		return;
	}

	srules.typename_allowed = false;
	srules.expecting_identifier = true;

	auto expression = cec::Compiler_ParseExpression(';', it_);
	 
	std::list<var_declr_data> data;

	auto it = expression.begin();
	auto end = expression.end();

	EvaluateDeclarationOperators(it, end, data);

	auto expr_it = it;

	std::list<expr::expression_token> tokens;
	TokenizeExpression(it, end, tokens);
	auto t_it = tokens.begin();

	if (tokens.front().tokentype != VarType::VT_STRING) {
		throw std::exception("expected an identifier");
	}

	if (tokens.size() > 1) {
		if((++t_it)->op && t_it->content != "=" || !t_it->op)
		{
			throw std::exception("expected a ';'");
		}
	}

	
	const std::string vname = tokens.front().content;
	const VarType vtype = GetDataType(type);

	auto var = DeclareVariable(vname, vtype);

	for (const auto& i : data) {
		if (i.arr)
			i.arr->type = vtype;
		SetVariableModifier(i, var);
	}


	expr::EvaluateEntireExpression(std::string(expr_it, end)); //now it can be evaluated since it's been pushed to the stack

}

int decl::EvaluateArrayInitialSize(const std::string& expression)
{

	if (expression.empty())
		return 1;

	auto result = expr::EvaluateEntireExpression(expression);

	if (!result.is_integral()) {
		throw std::exception("expression must be convertible to an integral type");
	}

	srules.reset();
	syntax.ClearFlags();
	expr::rules.reset();

	return result.get_int();


}
void decl::EvaluateDeclarationOperators(std::string::iterator& it, std::string::iterator& end, std::list<var_declr_data>& datalist)
{

	const token_t token = cec::Compiler_ReadToken(it, ';', end);
	var_declr_data data;



	if (token.eof_character) {
		throw std::exception("EvaluateDeclarationOperators(): check this out!!!!");
	}

	auto t = token.t_type;

	if (t == token_t::tokentype::DIGIT || t == token_t::tokentype::STRING) {
		it -= token.value.size();
		return;
	}

	if(t == token_t::WHITESPACE)
		return EvaluateDeclarationOperators(it, end, datalist);

	if (t != token_t::OPERATOR) {
		throw std::exception("expected an identifier or operator");
	}
	if (token.value == "[") {
		data.declaration_type = declr_type::ARRAY;
		data.arr = std::shared_ptr<array_declr_data>(new array_declr_data);
		std::string array_expression = ParseArrayExpression(it, end);
		data.arr->numElements = EvaluateArrayInitialSize(array_expression);
		datalist.push_front(data); // to the front because the evaluation is from right to left
		return EvaluateDeclarationOperators(it, end, datalist);
	}

	throw std::exception("unsupported type operator");


}

std::string decl::ParseArrayExpression(std::string::iterator& it, std::string::iterator& end)
{
	std::string array_expression;
	bool found = false;
	while (it != end) {
		if (*it == ']') {
			found = true;
			break;
		}

		array_expression.push_back(*it++);
	}

	if (!found) {
		throw std::exception("expected \"]\"");
	}
	++it; //skip the ]
	return array_expression;
}

void decl::SetVariableModifier(const var_declr_data& data, Variable* target)
{
	if (!target) {
		throw std::exception("SetVariableModifier(): impossible scenario");
	}

	std::function<Variable*(Variable*)> FindDeepestVariable = [&](Variable* var)
	{
		if (!var->reference && !var->arr.get())
			return var;

		if(var->reference){
			return FindDeepestVariable(var->reference);
		}

		return FindDeepestVariable(var->arr.get());
			
	};

	auto deepest = FindDeepestVariable(target);

	switch (data.declaration_type) {

	case declr_type::ARRAY:
		deepest->arr = std::shared_ptr<Variable[]>(new Variable[data.arr->numElements]);
		deepest->numElements = data.arr->numElements;
		for (int i = 0; i < data.arr->numElements; i++) {
			deepest->arr[i].set_type(target->get_type());
			deepest->arr[i].AllocateValues();
		}
		std::cout << "allocating an array of size " << data.arr->numElements << '\n';
		break;


	}
}