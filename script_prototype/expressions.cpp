#include "pch.h"

void Expression::TokenizeExpression(const std::string_view& expr_str, expression_s* expr)
{

	bool Operand_processed = FALSE; //true after parsing the expression operators 
	std::string token;
	int32_t idx = -1;
	char p;
	for (auto& i : expr_str) {
		idx++;
		if (!Operand_processed) {

			const bool isOperandCharacter = (i == '=');

			//first operand
			if (isOperandCharacter) {
				

				p = expr_str[idx < 1 ? 0 : idx - 1]; //p = previous character

				if(p == '+' || p == '-' || p == '=' || p == '/' || p == '*')
					expr->Operand.push_back(p);

				expr->Operand.push_back(i);


				p = expr_str[idx + 1 < expr_str.size() ?  idx + 1 : expr_str.size()  - 1]; //next character

				if (p == '+' || p == '-' || p == '=' || p == '/' || p == '*')
					expr->Operand.push_back(p);

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
//I think this should work... for now
ExpressionType Expression::EvaluateExpressionType(const std::string_view& operand)
{
	if (operand.find('=') != std::string_view::npos) //has =
		return ExpressionType::EXPR_ASSIGNMENT;


	return ExpressionType::EXPR_CALCULATION;


}
bool Expression::ParseExpression(std::string& expr)
{
	expression_s expression;

	TokenizeExpression(expr, &expression);

	expression.type = EvaluateExpressionType(expression.Operand);

	switch (expression.type) {
	case ExpressionType::EXPR_ASSIGNMENT:
		CompilerError("Assignment expressions are not supported yet");
		break;

	case ExpressionType::EXPR_CALCULATION:
		//1. remove blanks
		//2. check that all characters are numbers and parenthesis
		//3. the expression is ok and ParseExpressionNumbers can be called

		expr = RemoveBlank(expr);

		for (const auto& i : expr) {
			if (!std::isalnum(i) && i != '(' && i != ')' && i != '+' && i != '-' && i != '/' && i != '*') {
				CompilerError("Illegal character '", i, "' used in expression");
				return false;
			}
		}
		//int a = 1 + "aaa";
		break;

	}


	//called AFTER parsing the expression
	ParseExpressionNumbers(expr);

	return true;
}

//variables and functions are not allowed here
//only expects operands, parenthesis and numbers
//an expression like: 101 * (100 + ((2 + 3) * 3 + 4)) * (2 / 4 * (2 + 3 / (30 - 2 * 3)) / 2) can be used here
bool Expression::ParseExpressionNumbers(std::string& expr)
{
	
	//1. find parenthesis																			-> 2 * (2 + 3)
	//2. call EvaluateExpression with the expression within the parenthesis							-> 2 + 3
	//3. convert the original parenthesis expression to the return value from EvaluateExpression	-> (2 + 3) converts to 5
	//4. resulting string would be:																	-> 2 * 5

	const Parenthesis_s par = GetStringWithinParenthesis(expr);

	if (par.count_opening != par.count_closing) {
		CompilerError("mismatching parenthesis");
		return false;
	}

	if (!par.result_string.empty()) {
		const float result = EvaluateExpression(par.result_string);
		//std::cout << std::format("str: {} = {}\n", par.result_string, result);
		
		//replace the old expression with the new result
		expr.erase(par.opening, par.strlength+2);
		expr.insert(par.opening, std::to_string(result));

		//recursively calculate all parenthesis
		ParseExpressionNumbers(expr);
		return true;
	}
	
	 std::cout << "end: " << EvaluateExpression(expr) << '\n';
	 return true;
}

// ONLY PASS NUMBERS
// ALLOWED CHARACTERS: + - / *
float Expression::EvaluateExpression(const std::string_view& expression)
{


	std::vector<std::string> tokens;
	std::vector<expression_stack> expressionstack;


	size_t const opTokens = TokenizeStringOperands(expression, tokens);

	if (opTokens % 2 == 0) {
		CompilerError("Expected an expression");
		return false;
	}

	int i = 0;
	const size_t size = tokens.size() - 1;

	for (i = 0; i < size; i += 2) {
		expressionstack.push_back({ tokens[i], tokens[i+1]}); 
	}
	expressionstack.push_back({ tokens[size], "" });

	return EvaluateExpressionStack(expressionstack);
}
float Expression::EvaluateExpressionStack(std::vector<expression_stack>& es)
{


	std::vector<float> values;
	const size_t size = es.size();
	for (int i = 0; i < size; i++) {
		try {
			values.push_back(std::stof(es[i].content));

			std::cout << std::format("value[{}]: {}\n", i, values[i]);
		}
		catch (const std::exception& ex) {
			CompilerError(std::format("EvaluateExpressionStack(): {}\nexpression: {}", ex.what(), es[i].content));
		}
	}

	//ok now we have:
	//sequence of floating point numbers
	//sequence of operands

	OperatorPriority op, next_op;


	float result(0);
	int i;
	size_t vals = values.size();

	while(vals > 1){

		i = 0;

		op = GetOperandPriority(es[i].Operator[0]);
		next_op = GetOperandPriority(es[i+1].Operator[0]);


		while (next_op > op) {
			op = GetOperandPriority(es[i].Operator[0]);
			next_op = GetOperandPriority(es[i + 1].Operator[0]);
			if (next_op <= op)
				break;
			i++;
		}

		result = Eval(values[i], values[i + 1], es[i].Operator[0]);

		std::cout << std::format("{} {} {}\n", values[i], es[i].Operator[0], values[i + 1]);

		values.erase(values.begin() + i, values.begin()+i+1);
		es.erase(es.begin() + i, es.begin() + i + 1);

		values[i] = result;
		vals--;

	}
	return result;

}