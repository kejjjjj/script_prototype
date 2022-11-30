#include "pch.h"

void Expression::TokenizeExpression(expression_s* expr)
{

	bool Operand_processed = FALSE; //true after parsing the expression operators 
	std::string token;
	int32_t idx = -1;
	for (auto& i : expression_str) {
		idx++;
		if (!Operand_processed) {

			const bool isOperandCharacter = (i == '=');

			//first operand
			if (isOperandCharacter) {
				

				char p = expression_str[idx < 1 ? 0 : idx - 1]; //p = previous character

				if(p == '+' || p == '-' || p == '=' || p == '/' || p == '*')
					expr->Operand.push_back(p);

				expr->Operand.push_back(i);


				p = expression_str[idx + 1 < expression_str.size() ?  idx + 1 : expression_str.size()  - 1]; //next character

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
// ONLY PASS NUMBERS
// ALLOWED CHARACTERS: + - / *
bool Expression::EvaluateExpression()
{

	expression_s expression;

	TokenizeExpression(&expression);

	std::vector<std::string> tokens;
	std::vector<expression_stack> expressionstack;


	size_t opTokens = TokenizeStringOperands(expression.preOP, tokens);

	if (opTokens == NULL || opTokens % 2 == 0) {
		CompilerError("Expected an expression");
		return false;
	}

	for (int i = 0; i < (int)tokens.size()-1; i += 2) {

		expressionstack.push_back({ tokens[i], tokens[i+1]});

	}
	expressionstack.push_back({ tokens[tokens.size() - 1], "" });

	for (auto& i : expressionstack) {
		std::cout << std::format("{} -> {} | ", i.content, i.Operator);
	}
	std::cout << '\n';

	EvaluateExpressionStack(expressionstack);


	int a = 1 + 1 * 3 * 4;

	return true;
}
float Expression::EvaluateExpressionStack(std::vector<expression_stack>& es)
{


	std::vector<float> values;

	for (int i = 0; i < es.size(); i++) {
		try {
			values.push_back(std::stof(es[i].content));

			//std::cout << std::format("value[{}]: {}\n", i, values[i]);
		}
		catch (std::exception& ex) {
			std::cout << "EvaluateExpressionStack(): " << ex.what() << '\n';
		}
	}

	//ok now we have:
	//sequence of floating point numbers
	//sequence of operands

	OperatorPriority op, next_op;


	size_t amount_calculated = 0;
	float next_number;
	while(values.size() > 1){

		int i = 0;

		op = GetOperandPriority(es[i].Operator[0]);
		next_op = GetOperandPriority(es[i+1].Operator[0]);


		while (next_op > op) {
			op = GetOperandPriority(es[i].Operator[0]);
			next_op = GetOperandPriority(es[i + 1].Operator[0]);
			if (next_op <= op)
				break;
			i++;
		}

		float result = Eval(values[i], values[i + 1], es[i].Operator[0]);

		std::cout << std::format("{} {} {}\n", values[i], es[i].Operator[0], values[i + 1]);
		amount_calculated++;

		values.erase(values.begin() + i, values.begin()+i+1);
		es.erase(es.begin() + i, es.begin() + i + 1);

		values[i] = result;

	}
	constexpr float a = 101.f + 2.f * 3.f / 1.f * 3.f + 2.f - 30.f * 2.f - 5.f * 25.f / 2.f * 4.f - 4.f + 24.f / 2.f * 3.f;
	std::cout << "end result: " << values[0] << '\n';

	return 0;

}