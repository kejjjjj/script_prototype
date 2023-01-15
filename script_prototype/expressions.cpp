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

			const bool isOperatorCharacter = (i == '=');

			//first operator
			if (isOperatorCharacter) {
				

				p = expr_str[idx < 1 ? 0 : idx - 1]; //p = previous character

				if(p == '+' || p == '-' || p == '=' || p == '/' || p == '*' || p == '>' || p == '<' || p == '!' || p == '&')
					expr->Operator.push_back(p);

				expr->Operator.push_back(i);


				p = expr_str[idx + 1 < expr_str.size() ?  idx + 1 : expr_str.size()  - 1]; //next character

				if (p == '+' || p == '-' || p == '=' || p == '/' || p == '*' || p == '>' || p == '<' || p == '!' || p == '&')
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
//evaluates based on the op combo
//NOT called within expressions after the operand <----- FIX ME

ExpressionType Expression::EvaluateExpressionType(const std::string_view& Operator)
{
	size_t pos = Operator.find('=');
	if (pos != std::string_view::npos) {//has =

		if (Operator.size() == 1) { // = is the only op
			return ExpressionType::EXPR_ASSIGNMENT;
		}
		if(Operator[pos - 1] == '+' ||		//	+=
			Operator[pos - 1] == '-' ||		//	-=
			Operator[pos - 1] == '*' ||		//	*=
			Operator[pos - 1] == '/' ||		//	/=
			Operator[pos - 1] == '&' ||		//	&=
			Operator[pos - 1] == '|' ||		//	|=
			Operator[pos - 1] == '%' ||		//	%=
			Operator[pos - 1] == '^')		//	^=

			return ExpressionType::EXPR_ASSIGNMENT;

	}

	//std::cout << "expression type: EXPR_CALCULATION\n";
	return ExpressionType::EXPR_CALCULATION;


}
//checks if an op combo is allowed
bool Expression::NextOperatorIsLegal(char previous_op, char op)
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
		return op == '=' || op == '<' || (op == '-');	//	<= || << || <-
	case '>':
		return op == '=' || op == '>' ||(op == '-');	//	>= || >> || >-
	case '&':
		return op == '&' || (op == '-');				//	&& || &-
	case '|':
		return op == '|' || (op == '-');				//	|| || |-
	case '~':
		return false;									//	~
	case '^':
		return (op == '-');								//	^  || ^-
	case '!':
		return op == '=' || (op == '-');				//	!= || !-
	case '%':
		return op == '=' || (op == '-');				//	%  || %-

	}

	//int a = 2^-6;

	return false;
}

bool Expression::ParseExpression(std::string& expr)
{
//	std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();

	TokenizeExpression(expr, &e.expression);
	
	//std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	//std::chrono::duration<double> difference = now - old;


	//printf("tokenize expression took: %.12f\n", difference.count());
	//CompilerError("1");

	e.expression.type = EvaluateExpressionType(e.expression.Operator);
	e.operands = e.expression.Operator.size();

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
					CompilerError("Illegal amount of expression operators");
					return false;
				}
				if (!NextOperatorIsLegal(last_character, i)) {
					CompilerError("Illegal operator sequence '", last_character, i, "'");
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
		//constexpr int a = (3 << 2) * 3 <= (2 ^ -5 < 3 % 1 * 7 - 2 % 3 | (2 / 3) && (1 || 3)) * 4 < 2 > 3 == 20 != 12 * 3 >> 3 & 5;
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
	
	//1. find parentheses																			-> 2 * (2 + 3)
	//2. call EvaluateExpression with the expression within the parentheses							-> 2 + 3
	//3. convert the original parenthesis expression to the return value from EvaluateExpression	-> (2 + 3) converts to 5
	//4. resulting string would be:																	-> 2 * 5

	const Parenthesis_s par = GetStringWithinParentheses(expr);

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
	//std::cout << "result: " << result << '\n';
	return result;
}

// expects an expression that does not include parantheses
// example: 1 ^ 3 & 1 / 5
float Expression::EvaluateExpression(const std::string_view& expression)
{

	if (ValidNumber(expression)) {
		return std::stof(std::string(expression));
	}

	std::list<std::string> tokens;
	std::list<expression_stack> expressionstack;

	std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();


	size_t const opTokens = TokenizeStringOperands(expression, tokens);

	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	std::chrono::duration<float> difference = now - old;

	printf("time taken: %.6f\n", difference.count());

	if (opTokens % 2 == 0) {
		CompilerError("Expected an expression");
		return false;
	}

	int i = 0;
	const size_t size = tokens.size() - 1;

	std::list<std::string>::iterator 
		it = tokens.begin(), 
		it_next = tokens.begin();

	std::advance(it_next, 1); //next token

	for (i = 0; i < size; i += 2) {

		expressionstack.push_back({ *it, *it_next}); 

		std::advance(it, 2);
		std::advance(it_next, 2);
	}
	//it = tokens.end();
	expressionstack.push_back({ *it, "" });

	float ret = EvaluateExpressionStack(expressionstack);
	//std::cout << "sizeof expressionstack: " << expressionstack.size() << '\n';
	return ret;
}
float Expression::EvaluateExpressionStack(std::list<expression_stack>& es)
{

	std::list<float> values;
	const size_t size = es.size();

	for (const auto& i : es) {
		try {
			values.push_back(std::stof(i.content));

			//std::cout << std::format("value[{}]: {}\n", i, values[i]);
		}
		catch (const std::exception& ex) {
			CompilerError(std::format("EvaluateExpressionStack(): {}\nexpression: {}", ex.what(), i.content));
		}
	}


	//ok now we have:
	//sequence of floating point numbers
	//sequence of operators

	OperatorPriority op, next_op;


	float result(0);
	int i=0;
	size_t vals = size;
	std::list<float>::iterator val_itr1, val_itr2;
	std::list<expression_stack>::iterator es_itr1, es_itr2, es_begin1;

	//std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();

	//order of operations (PEMDAS)
	while(vals > 1){

		val_itr1 = values.begin();
		val_itr2 = val_itr1;
		std::advance(val_itr2, 1); //next operator

		es_itr1 = es.begin();
		es_itr2 = es_itr1;
		std::advance(es_itr2, 1); //next operator


		i = 0;

		op = GetOperandPriority(es_itr1->Operator);
		next_op = GetOperandPriority(es_itr2->Operator);


		//move to the right if the operand priority is higher
		while (next_op > op) {

			op = GetOperandPriority(es_itr1->Operator);
			next_op = GetOperandPriority(es_itr2->Operator); //i+1 is always in range unless this function was called with bad arguments
			if (next_op <= op)
				break;

			i++;
			std::advance(es_itr1, 1);
			std::advance(es_itr2, 1); //always one iteration ahead
			
		}

		std::advance(val_itr1, i);
		std::advance(val_itr2, i);

		result = Eval(*val_itr1, *val_itr2, es_itr1->Operator);

		//std::cout << std::format("{} {} {} = {}\n", *val_itr1, es_itr1->Operator, *val_itr2, result);

		values.erase(val_itr1, val_itr2);

		es.erase(es_itr1, es_itr2);


		*(++val_itr1) = result;
		vals--;

	}
	//std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	//std::chrono::duration<float> difference = now - old;

	//printf("time taken: %.6f\n", difference.count());
	return result;

}