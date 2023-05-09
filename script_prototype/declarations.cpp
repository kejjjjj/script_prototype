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
	 
	var_declr_data data;

	auto it = expression.begin();
	auto end = expression.end();

	while (std::isspace(*it))
		if (++it == end)
			throw std::exception("expected an expression");

	data.declaration_type = DeclarationUnaryToType(*it);
	const bool bArray = data.declaration_type == declr_type::ARRAY;


	if (bArray) {
		data.arr = std::shared_ptr<array_declr_data>(new array_declr_data);
		data.arr->numElements = EvaluateArrayInitialSize(expression);
		data.arr->type = (VarType)GetDataType(data.variable_type);

		std::cout << "creating an array of size " << data.arr->numElements << '\n';

	}
	
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

	

	data.variable_type = type;
	data.variable_name = tokens.front().content;




	DeclareVariable(data);

	expr::EvaluateEntireExpression(expression); //now it can be evaluated since it's been pushed to the stack

}

int decl::EvaluateArrayInitialSize(const std::string& expression)
{

	auto result = expr::EvaluateEntireExpression(expression);

	if (!result.is_integral()) {
		throw std::exception("expression must be convertible to an integral type");
	}

	srules.reset();
	syntax.ClearFlags();
	expr::rules.reset();

	return result.get_int();


}
void decl::EvaluateDeclarationOperators(std::string::iterator& it, std::string::iterator& end)
{

	const token_t token = cec::Compiler_ReadToken(it, ';', end);

	if (token.eof_character) {
		throw std::exception("EvaluateDeclarationOperators(): check this out!!!!");
	}

	auto t = token.t_type;

	if (t == token_t::tokentype::DIGIT || t == token_t::tokentype::STRING)
		return;

	if(t == token_t::WHITESPACE)
		return EvaluateDeclarationOperators(it, end);

	if (t != token_t::OPERATOR) {
		throw std::exception("expected an identifier or operator");
	}

	if (token.value == "[") {
		std::string array_expression = ParseArrayExpression(it, end);
		int arrsize = EvaluateArrayInitialSize(array_expression);
		return EvaluateDeclarationOperators(it, end);
	}



}

std::string ParseArrayExpression(std::string::iterator& it, std::string::iterator& end)
{
	std::string array_expression;
	bool found = false;
	while (it != end) {
		if (*it == ']') {
			found = true;
			break;
		}

		array_expression.push_back(*++it);
	}

	if (!found) {
		throw std::exception("expected \"]\"");
	}

	return array_expression;
}