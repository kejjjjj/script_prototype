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
	auto begin = expression.find('[');
	auto end = expression.find(']');

	if (end == std::string::npos) {
		throw std::exception("expected a \"]\"");
	}

	auto result = expr::EvaluateEntireExpression(expression.substr(begin+1, end-1));

	if (!result.is_integral()) {
		throw std::exception("expression must be convertible to an integral type");
	}

	srules.reset();
	syntax.ClearFlags();
	expr::rules.reset();

	return result.get_int();


}