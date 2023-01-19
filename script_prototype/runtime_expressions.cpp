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

	for (const auto& i : expr_str) {
		idx++;
		if (!Operand_processed) {


			const bool isOperatorCharacter = (i == '=');

			//first operator
			if (isOperatorCharacter) {
				p = expr_str[idx < 1 ? 0 : idx - 1]; //p = previous character

				if (IsOperator(p))
					expr->Operator.push_back(p);

				expr->Operator.push_back(i);


				p = expr_str[idx + 1 < expr_str.size() ? idx + 1 : expr_str.size() - 1]; //next character

				if (IsOperator(p))
					expr->Operator.push_back(p);

				if (expr_str[idx - 1] == '!')// operand is !=
					token.pop_back(); // remove the ! character from the back

				expr->preOP = token; //store left side

				token.clear();

				Operand_processed = true;

				continue;
			}

		}
		token.push_back(i);

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

	size_t pos = expr->Operator.find('=');
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

			return ExpressionType::EXPR_ASSIGNMENT;

	}

	//std::cout << "expression type: EXPR_CALCULATION\n";
	return ExpressionType::EXPR_CALCULATION;


}
bool RuntimeExpression::ParseExpression(std::string& expr)
{
	TokenizeExpression(expr, &e.expression);

	e.expression.type = EvaluateExpressionType(&e.expression);
	e.operands = e.expression.Operator.size();

	switch (e.expression.type) {

	case ExpressionType::EXPR_ASSIGNMENT:
		ParseAssignment();
		break;
	case ExpressionType::EXPR_CALCULATION:
		break;
	default:
		RuntimeError("Unable to validate expression");
		return false;
	}


	ParseExpressionNumbers(expr);


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

		
		////replace the old expression with the new result
		//expr.erase(par.opening, par.strlength + 2);
		//expr.insert(par.opening, std::to_string(result));

		ParseExpressionNumbers(expr);



		return 1;

	}


}
// expects an expression that does not include parantheses
// example: 1 ^ 3 & 1 / 5
std::string RuntimeExpression::EvaluateExpression(const std::string_view& expression)
{
	if (ValidNumber(expression)) {
		return std::string(expression);
	}

	std::list<std::string> tokens;
	std::list<expression_stack> expressionstack;


	size_t const opTokens = TokenizeStringOperands(expression, tokens); //separates all operators

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


}
//variable names can be passed
std::string RuntimeExpression::EvaluateExpressionStack(std::list<expression_stack>& es)
{
	const auto begin = es.begin();
	auto end = es.end();

	std::advance(end, -1); //ignore last because the operator is ""
	VarType leftop{}, rightop{};
	OperatorPriority opriority;

	size_t vals = es.size();

	while (vals > 1) {



		vals--;
	}

}
//this function ONLY expects numbers or strings
//variables are not expected
std::string Eval(const std::string_view& a, const std::string_view& b, const std::string_view& ops)
{

}