#include "pch.h"

void Expression::TokenizeExpression(std::string& expr_str, expression_s* expr)
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

bool Expression::ParseExpression(std::string& expr)
{
	expression_s expression;

	TokenizeExpression(expr, &expression);

	ParseExpressionNumbers(expr);
}

//variables and functions are not allowed here
//only expects operands, parenthesis and numbers
bool Expression::ParseExpressionNumbers(std::string& expr)
{
	
	//1. find parenthesis																			-> 2 * (2 + 3)
	//2. call EvaluateExpression with the expression within the parenthesis							-> 2 + 3
	//3. convert the original parenthesis expression to the return value from EvaluateExpression	-> (2 + 3) converts to 5
	//4. resulting string would be:																	-> 2 * 5

	Parenthesis_s par = GetStringWithinParenthesis(expr);

	if (par.count_opening != par.count_closing) {
		CompilerError("mismatching parenthesis");
		return false;
	}

	if (!par.result_string.empty()) {
		float result = EvaluateExpression(par.result_string);
		std::cout << std::format("str: {} = {}\n", par.result_string, result);
		
		int closing = par.count_closing;

		//!!!!!!!!!!!FIX THIS!!!!!!!!!!!!!!!
		expr.erase(par.opening, closing);



		expr.insert(par.opening, std::to_string(result));

		ParseExpressionNumbers(expr);
	}

	
	EvaluateExpression(expr);
	return true;
}

// ONLY PASS NUMBERS
// ALLOWED CHARACTERS: + - / *
float Expression::EvaluateExpression(std::string& expression)
{


	std::vector<std::string> tokens;
	std::vector<expression_stack> expressionstack;


	size_t const opTokens = TokenizeStringOperands(expression, tokens);

	if (opTokens == NULL || opTokens % 2 == 0) {
		CompilerError("Expected an expression");
		return false;
	}

	int i = 0;
	for (i = 0; i < tokens.size()-1; i += 2) {
		expressionstack.push_back({ tokens[i], tokens[i+1]}); 
	}
	expressionstack.push_back({ tokens[tokens.size() - 1], "" });

	return EvaluateExpressionStack(expressionstack);
}
float Expression::EvaluateExpressionStack(std::vector<expression_stack>& es)
{


	std::vector<float> values;

	for (int i = 0; i < es.size(); i++) {
		try {
			values.push_back(std::stof(es[i].content));

		//	std::cout << std::format("value[{}]: {}\n", i, values[i]);
		}
		catch (std::exception& ex) {
			CompilerError(std::format("EvaluateExpressionStack(): {}", ex.what()));
		}
	}

	//ok now we have:
	//sequence of floating point numbers
	//sequence of operands

	OperatorPriority op, next_op;

	float result(0);
	int i = 0;
	while(values.size() > 1){

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

	}
	return result;

}