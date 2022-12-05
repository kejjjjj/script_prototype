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

				if(p == '+' || p == '-' || p == '=' || p == '/' || p == '*' || p == '>' || p == '<' || p == '!')
					expr->Operand.push_back(p);

				expr->Operand.push_back(i);


				p = expr_str[idx + 1 < expr_str.size() ?  idx + 1 : expr_str.size()  - 1]; //next character

				if (p == '+' || p == '-' || p == '=' || p == '/' || p == '*' || p == '>' || p == '<' || p == '!')
					expr->Operand.push_back(p);

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
//evaluates based on the op combo
//NOT called within expressions after the operand <----- FIX ME

ExpressionType Expression::EvaluateExpressionType(const std::string_view& operand)
{
	size_t pos = operand.find('=');
	if (pos != std::string_view::npos) {//has =

		if (operand.size() == 1) { // = is the only op
			return ExpressionType::EXPR_ASSIGNMENT;
		}
		if( operand[pos - 1] == '+' ||		//	+=
			operand[pos - 1] == '-' ||		//	-=
			operand[pos - 1] == '*' ||		//	*=
			operand[pos - 1] == '/' ||		//	/=
			operand[pos - 1] == '&' ||		//	&=
			operand[pos - 1] == '|' ||		//	|=
			operand[pos - 1] == '^')		//	^=

			return ExpressionType::EXPR_ASSIGNMENT;

	}

	std::cout << "expression type: EXPR_CALCULATION\n";
	return ExpressionType::EXPR_CALCULATION;


}
//checks if an op combo is allowed
// -= is allowed
// +- is not allowed

bool Expression::NextOperandIsLegal(char previous_op, char op)
{
	if (previous_op == '\0') //no previous character 
		return true;

	switch (previous_op) {
	case '+':
		return op == '=' || (op == '+' || op == '-');	//	+= || ++ || +-
	case '-':
		return op == '=' || (op == '-');				//	-= || --
	case '*':
		return op == '=' || (op == '-');				//	*= || *- 
	case '/':
		return op == '=' || (op == '-');				//	/= || /- 
	case '<':
		return op == '=' || (op == '-');				//	<= || <-
	case '>':
		return op == '=' || (op == '-');				//	>= || >-
	case '&':
		return op == '&' || (op == '-');				//	&& || &-
	case '|':
		return op == '|' || (op == '-');				//	|| || |-
	case '~':
		return false;									//	~
	case '^':
		return (op == '-');								//	^ || ^-
	case '!':
		return op == '=' || (op == '-');				//	!= || !-

	}

	//int a = 2^-6;

	return false;
}

bool Expression::ParseExpression(std::string& expr)
{
	TokenizeExpression(expr, &e.expression);

	e.expression.type = EvaluateExpressionType(e.expression.Operand);
	e.operands = e.expression.Operand.size();

	switch (e.expression.type) {
	case ExpressionType::EXPR_ASSIGNMENT:
		CompilerError("Assignment expressions are not supported yet\nType: ", (int)e.expression.type);
		break;

	case ExpressionType::EXPR_CALCULATION:
		//1. remove blanks
		//2. check that all characters are numbers || parenthesis || operators
		//3. sanity checks
		//4. the expression is ok and ParseExpressionNumbers can be called

		
		//expr = RemoveBlank(expr);
		char last_character{'\0'};
		int32_t operands_in_a_row{0}, idx{-1};


		for (const auto& i : expr) {
			idx++;
			
			//pre-, and post-increments here probably...
			// because ++ != + +
			
			//idk I will probably do this when I actually have variable support


			if (std::isblank(i))
				continue;

			if (!std::isalnum(i) && BadCalculationOp(i)) {
				CompilerError("Illegal character '", i, "' used in expression");
				return false;
			}


			if (IsCalculationOp(i)) {
				if (operands_in_a_row > 2) {
					CompilerError("Illegal amount of expression operands");
					return false;
				}
				if (!NextOperandIsLegal(last_character, i)) {
					CompilerError("Illegal operand sequence '", last_character, i, "'");
					return false;
				}

				last_character = i;
				operands_in_a_row++;
				continue;
			}
			operands_in_a_row = false;

			//+(+ == ++
			//if(i != '(' && i != ')')
				last_character = '\0';
		}
		constexpr int a = (2 != 1 * 3) * 2 ^ 3 < (4 + 500 | 2) ^ 3 == 4 & 123 + (1 < 4 * 7);

		break;

	}

	expr = RemoveBlank(expr);

	//called AFTER parsing the expression
	float result = ParseExpressionNumbers(expr);
	//std::cout << "end: " << result << '\n';

	return true;
}

//variables and functions are not allowed here
//only expects operands, parenthesis and numbers
//an expression like: 101 * (100 + ((2 + 3) & 3 ^ 4)) * (2 | 4 * -(-2 + 3 / (30 - 2 | 3)) ^ 2) can be used here
float Expression::ParseExpressionNumbers(std::string& expr)
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
		
		
		//replace the old expression with the new result
		expr.erase(par.opening, par.strlength+2);
		expr.insert(par.opening, std::to_string(result));

		ParseExpressionNumbers(expr);
			
		
		return 1;
	}


	//std::cout << "last expression: " << expr << '\n';
	const float result = EvaluateExpression(expr);
	std::cout << "result: " << result << '\n';
	return result;
}

// ONLY PASS NUMBERS
// ALLOWED CHARACTERS: + - / *
float Expression::EvaluateExpression(const std::string_view& expression)
{

	if (ValidNumber(expression)) {
		return std::stof(std::string(expression));
	}

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
	for (size_t i = 0; i < size; i++) {
		try {
			values.push_back(std::stof(es[i].content));

			//std::cout << std::format("value[{}]: {}\n", i, values[i]);
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

	//order of operations (PEMDAS)
	while(vals > 1){

		i = 0;

		op = GetOperandPriority(es[i].Operator);
		next_op = GetOperandPriority(es[i+1].Operator);

		//move to the right if the operand priority is higher
		while (next_op > op) {
			op = GetOperandPriority(es[i].Operator);
			next_op = GetOperandPriority(es[i + 1].Operator); //i+1 is always in range unless this function was called with bad arguments
			if (next_op <= op)
				break;
			i++;
		}

		//if (es[i].Operator.size() != 1) {
		//	CompilerError("hi I haven't added support for two consecutive operators yet!");
		//	return false;
		//}

		result = Eval(values[i], values[i + 1], es[i].Operator);

		std::cout << std::format("{} {} {} = {}\n", values[i], es[i].Operator, values[i + 1], result);

		values.erase(values.begin() + i, values.begin()+i+1);
		es.erase(es.begin() + i, es.begin() + i + 1);

		values[i] = result;
		vals--;

	}
	return result;

}