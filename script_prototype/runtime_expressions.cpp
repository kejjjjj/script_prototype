#include "pch.h"

//converts the expression to three parts
//1. the expression before the assignment operator
//2. the operator (if exists)
//3. the expression after the assignment operator (if exists)
void RuntimeExpression::TokenizeExpression(const std::string_view& expr_str, expression_s* expr)
{

	bool Operand_processed = FALSE; //true after parsing the expression operators 
	std::string token;
	int32_t idx = -1;
	char p;

	auto begin = expr_str.begin();
	auto end = expr_str.end();

	const size_t size = expr_str.size();

	for (auto it = begin; it != end; it++) {
		auto& i = *it;
		idx++;
		const bool isOperatorCharacter = (i == '=');
		if (Operand_processed || !isOperatorCharacter) {
			token.push_back(i);
			continue;
		}
		


		if (it == end || it == begin) {
			RuntimeError("unexpected end of expression\n");
			return;
		}

		p = *(it - 1); //p = previous character

		if (IsOperator(p)) {
			expr->Operator.push_back(p); // ?= is true here
			Operand_processed = true;
			token.pop_back();
		}expr->Operator.push_back(i); 



		if (!Operand_processed) {
			p = *(it + 1); //next character

			if (IsPostEqualOp(p)) {
				expr->Operator.push_back(p);
				it++; //skip the next operator
			}
		}

		expr->preOP = token; //store left side

		token.clear();

		Operand_processed = true;

		continue;
		

	}

	if (expr->preOP.empty())
		expr->preOP = token;

	else
		expr->postOP = token;
}
ExpressionType RuntimeExpression::EvaluateExpressionType(expression_s* expr)
{
	for (const auto& i : VarTypes) {
		if (expr->preOP.find(std::string(i) + " ") != std::string::npos) {
			return ExpressionType::EXPR_ASSIGNMENT;
		}
	}

	const size_t pos = expr->Operator.find('=');
	if (pos != std::string_view::npos) {//has =

		if (expr->Operator.size() == 1) { // = is the only op
			return ExpressionType::EXPR_ASSIGNMENT;
		}
		if (expr->Operator[pos - 1] == '+' ||		//	+=
			expr->Operator[pos - 1] == '-' ||		//	-=
			expr->Operator[pos - 1] == '*' ||		//	*=
			expr->Operator[pos - 1] == '/' ||		//	/=
			expr->Operator[pos - 1] == '&' ||		//	&=
			expr->Operator[pos - 1] == '|' ||		//	|=
			expr->Operator[pos - 1] == '%' ||		//	%=
			expr->Operator[pos - 1] == '^')		//	^=

			return ExpressionType::EXPR_ASSIGNMENT2;

	}

	//std::cout << "expression type: EXPR_CALCULATION\n";
	return ExpressionType::EXPR_CALCULATION;


}
bool RuntimeExpression::ParseExpression(std::string& expr)
{
	//std::cout << "parsing: " << expr << '\n';
	TokenizeExpression(expr, &e.expression);

	e.expression.type = EvaluateExpressionType(&e.expression);
	e.operands = e.expression.Operator.size();

	switch (e.expression.type) {

	case ExpressionType::EXPR_ASSIGNMENT:
		ParseAssignment();
		expr = e.expression.postOP;
		break;
	case ExpressionType::EXPR_ASSIGNMENT2:
		ParseAssignment2();
		expr = e.expression.postOP;
		break;
	case ExpressionType::EXPR_CALCULATION:
		std::cout << "ExpressionType::EXPR_CALCULATION";

		break;
	default:
		RuntimeError("Unable to validate expression");
		return false;
	}


	ParseExpressionNumbers(expr);

	return 1;
}
bool RuntimeExpression::ParseAssignment()
{
	std::string variableName = e.expression.preOP;
	Variable _var;

	if (IsVariableInitialization(variableName)) {
	
		//check the initialization type
		const std::string varType = GetVariableTypeString(variableName);

		const size_t type = GetDataType(varType);

		_var.type = (VarType)type;
		_var.name = variableName.substr(varType.length() + 1); //the string after the type

		scriptStack.PushToStack(_var);
		var = &scriptStack.stack.back();


	}
	else {

		var = FindVariableFromStack(variableName);

		if (!var) {
			RuntimeError("undefined variable '", variableName, "' referenced!");
			return 0;
		}
	}

	return true;
}
//dual op assignment to a variable like +=
bool RuntimeExpression::ParseAssignment2() 
{
	std::string variableName = e.expression.preOP;
	
	var = FindVariableFromStack(variableName);

	if (!var) {
		RuntimeError("undefined variable '", variableName, "' referenced!");
		return 0;
	}


	return true;

}
//functions are not allowed here
//only expects operators, parenthesis, numbers and variable names
//an expression like: 101 * (100 + ((2 + 3) & 3 ^ 4)) * (2 | 4 * -(-2 + 3 / (30 - 2 | 3)) ^ 2) can be used here
bool RuntimeExpression::ParseExpressionNumbers(std::string& expr)
{
	//1. find parentheses																			-> 2 * (2 + 3)
	//2. call EvaluateExpression with the expression within the parentheses							-> 2 + 3
	//3. convert the original parenthesis expression to the return value from EvaluateExpression	-> (2 + 3) converts to 5
	//4. resulting string would be:																	-> 2 * 5

	const Parenthesis_s par = GetStringWithinParentheses(expr);
	
	//parentheses exists
	if (!par.result_string.empty()) {

		//evaluate the expression within the parentheses

		
		//replace the old expression with the new result
		//expr.erase(par.opening, par.strlength + 2);
		//expr.insert(par.opening, std::to_string(result));

		const std::string result = EvaluateExpression(par.result_string);

		expr.erase(par.opening, par.strlength + 2);
		expr.insert(par.opening, result);


		ParseExpressionNumbers(expr);



		return 1;

	}

	if(e.expression.type != ExpressionType::EXPR_ASSIGNMENT2)
		var->value = EvaluateExpression(expr);
	else {
		std::string temp = EvaluateExpression(expr);
		std::string op { (e.expression.Operator.front()) };
		var->value = Eval(var->value, temp, op);
	}

	std::cout << "value of '" << var->name << "' = " << var->value << '\n';

	return 1;
}
// expects an expression that does not include parantheses
// example: 1 ^ 3 & 1 / 5
std::string RuntimeExpression::EvaluateExpression(const std::string_view& expression)
{

	if (ValidNumber(expression) || expression.front() == '"' && expression.front() == expression.back()) {
		return std::string(expression);
	}

	std::list<std::string> tokens;
	std::list<expression_stack> expressionstack;

	std::cout << "RuntimeExpression: " << expression << '\n';

	size_t const opTokens = TokenizeStringOperands2(expression, tokens); //separates all operators

	if (tokens.size() == 1)
		return EvaluateSingular(tokens.front());

	const size_t size = tokens.size() - 1;

	std::list<std::string>::iterator
		it = tokens.begin(),
		it_next = tokens.begin();

	std::advance(it_next, 1); //next token

	for (int i = 0; i < size; i += 2) {

		expressionstack.push_back({ *it, *it_next });

		std::advance(it, 2);
		std::advance(it_next, 2);
	}

	expressionstack.push_back({ *it, "" });

	return EvaluateExpressionStack(expressionstack);
}
//variable names can be passed
std::string RuntimeExpression::EvaluateExpressionStack(std::list<expression_stack>& es)
{
	std::list<expression_stack>::iterator es_itr1, es_itr2;
	size_t vals = es.size();
	OperatorPriority op, next_op;
	std::string lval, rval;

	const auto ContentToValue = [](const std::string& str)->std::string
	{
		if (ValidNumber(str))
			return str;

		else if (GetCharacterCount(str, '"') == 2) {
			return str.substr(1, str.size() - 2);
		}

		std::string s = str;
		auto variable_prefix = HasPrefix(s);

		if (!variable_prefix.empty())
			s.erase(0, variable_prefix.size());

		const Variable* v = FindVariableFromStack(s);

		if (!v) {

			if (!ValidNumber(s)) {

				RuntimeError("Undefined variable '", s, "'");
				return "";
			}

			return EvalPrefixes(s, variable_prefix);
		}
		
		s = v->value;

		return EvalPrefixes(s, variable_prefix);
	};

	//ok now we have:
	//a list of tokenized operands and operators for easy parsing
	//every odd index is an operand
	//every even index is an operator


	while (vals > 1) {

		es_itr1 = es.begin();
		es_itr2 = es_itr1; ++es_itr2;

		op = GetOperandPriority(es_itr1->Operator);
		next_op = GetOperandPriority(es_itr2->Operator);

		while (next_op > op) {

			op = GetOperandPriority(es_itr1->Operator);
			next_op = GetOperandPriority(es_itr2->Operator); //i+1 is always in range unless this function was called with bad arguments
			if (next_op <= op)
				break;

			++es_itr1;
			++es_itr2;
		}

		lval = ContentToValue(es_itr1->content);
		rval = ContentToValue(es_itr2->content);

		const std::string result = Eval(lval, rval, es_itr1->Operator);
		std::cout << std::format("{} {} {} = {}\n", lval, es_itr1->Operator, rval, result);

		constexpr int a = !1 + -1;

		es.erase(es_itr1, es_itr2);
		es_itr2->content = result;
		vals--;
	}

	return es_itr2->content;
}

//no operators
//only one number/variable with possible prefixes
std::string RuntimeExpression::EvaluateSingular(const std::string& es)
{
	


	const auto ContentToValue = [](const std::string& str)->std::string
	{
		if (ValidNumber(str))
			return str;

		else if (GetCharacterCount(str, '"') == 2) {
			return str.substr(1, str.size() - 2); //is a string literal so remove the quotes
		}

		std::string s = str;
		auto variable_prefix = HasPrefix(s);

		if (!variable_prefix.empty())
			s.erase(0, variable_prefix.size()); //separate the prefix

		const Variable* v = FindVariableFromStack(s);

		if (!v) {

			if (!ValidNumber(s)) {

				RuntimeError("Undefined variable '", s, "'");
				return "";
			}

			return EvalPrefixes(s, variable_prefix);
		}
		
		s = v->value;

		return EvalPrefixes(s, variable_prefix);

	};

	return ContentToValue(es);

}


VarType RuntimeExpression::GetOperandType(const std::string_view& operand)
{
	//std::cout << "GetOperandType: " << operand << '\n';
	if (ValidNumber(operand)) {
		//either an int or a float

		if (IsInteger(operand))
			return VarType::VT_INT;

		return VarType::VT_FLOAT;

	}

	if (GetCharacterCount(operand, '"') == 2) {
		return VarType::VT_STRING;
	}

	//return VarType::VT_INVALID; //could still be a variable

	Variable* v = FindVariableFromStack(operand);

	if (!v) {
		RuntimeError("'", operand, "' is undefined");
		return VarType::VT_INVALID;
	}

	return (VarType)v->type;
}