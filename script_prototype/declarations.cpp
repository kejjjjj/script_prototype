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

	auto expr_it = it; //expr_it is now at the point AFTER the datatype

	std::list<expr::expression_token> tokens;
	TokenizeExpression(it, end, tokens, 2); //only read 2 tokens
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

	std::list<std::optional<int>> numChildren;

	for (const auto& i : data) {

		if (i.arr) {
			i.arr->type = vtype;
			numChildren.push_back(std::make_optional(i.arr->numElements));
			continue;
		}

		numChildren.push_back(std::nullopt);
	}

	PopulateVariableTree(var, numChildren);

	bool expecting_initializer = false;

	while (expr_it != end)
		if (expecting_initializer = cec::Compiler_ReadToken(expr_it, ';', end).value == "=")
			break;

	while (std::isspace(*expr_it) && expr_it != end)
		++expr_it;

	if (expr_it == end) { //no initializer
		
		if (expecting_initializer) {
			stack_variables.erase(var->name);
			throw std::exception("expected an initializer");
		}
		if (var->is_reference()) {
			stack_variables.erase(var->name);
			throw std::exception("a reference declaration requires an initializer");
		}

		return; 
	}

	init::SetVariableInitializer(*var, std::string(expr_it, end));
	
	//expr::EvaluateEntireExpression(std::string(expr_it, end)); //now it can be evaluated since it's been pushed to the stack
}

int decl::EvaluateArrayInitialSize(const std::string& expression)
{

	if (expression.empty()) {
		//throw std::exception("an empty array declaration is not allowed");
		return 1; 
	}

	auto result = expr::EvaluateEntireExpression(expression);

	if (!result.is_integral()) {
		throw std::exception("expression must be convertible to an integral type");
	}

	if (result.get_int() < 1) {
		throw std::exception("array size must be greater than 0");
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
		const std::string array_expression = ParseArrayExpression(it, end);
		data.arr->numElements = EvaluateArrayInitialSize(array_expression);
		datalist.push_back(data); // to the back because the evaluation is from left to right
		return EvaluateDeclarationOperators(it, end, datalist);
	}
	else if (token.value == "?") {
		data.reference = true;
		datalist.push_back(data); // to the back because the evaluation is from left to right
		return EvaluateDeclarationOperators(it, end, datalist);
	}

	throw std::exception("expected an identifier");


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

//children cannot be a copy
void decl::PopulateVariableTree(Variable* parent, std::list<std::optional<int>> children)
{
	if (children.empty())
		return;

	if(!children.front().has_value()){ // no value means a reference!

		if (parent->is_reference()) {
			throw std::exception("a reference to reference is not allowed");
		}
		children.erase(children.begin());

		parent->reference = std::shared_ptr<Variable>(new Variable);

		return PopulateVariableTree(parent->reference.get(), children);
	}
	//an array is assumed if this executes

	const auto numChildren = children.front().value();

	children.erase(children.begin());

	parent->arr = std::shared_ptr<Variable[]>(new Variable[numChildren]);
	parent->numElements = numChildren;

	for (int i = 0; i < numChildren; i++) {
		parent->arr[i].set_type(parent->get_type());
		parent->arr[i].AllocateValues();
		PopulateVariableTree(&parent->arr[i], children);
	}
}