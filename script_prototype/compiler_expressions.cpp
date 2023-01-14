#include "pch.h"

//1. remove blanks
//2. check that all characters are numbers || parentheses || operators
//returns "" if it fails
std::string CompilerExpression::CleanupExpression(const std::string_view& expr)
{

	char last_character{ '\0' };
	int32_t operands_in_a_row{ 0 }, idx{ -1 };

	for (const auto& i : expr) {
		idx++;

		//pre-, and post-increments here probably...
		// because ++ != + +

		//idk I will probably do this when I actually have variable support

		if (std::isspace(i))
			continue;

		if (!std::isalnum(i) && BadCalculationOp(i) && i != '\n' && i != '"') {

			if (i != expr.front()) { //is . used because of a decimal point?
				if (i == '.' && std::isdigit(expr[idx - 1])) { //if so, then skip
					goto isfine;
				}
			}

			CompilerError("Illegal character '", i, "' used in expression");
			return "";
		}
		isfine:

		if (IsCalculationOp(i)) {
			if (operands_in_a_row > 2) {
				CompilerError("Illegal amount of expression operators");
				return "";
			}
			if (!NextOperatorIsLegal(last_character, i)) {
				CompilerError("Illegal operator sequence '", last_character, i, "'");
				return "";
			}

			last_character = i;
			operands_in_a_row++;
			continue;
		}
		operands_in_a_row = false;
		last_character = '\0';
	}

	return RemoveIrrelevantCode(expr);
}
//converts the expression to three parts
//1. the expression before the assignment operator
//2. the operator (if exists)
//3. the expression after the assignment operator (if exists)
void CompilerExpression::TokenizeExpression(const std::string_view& expr_str, expression_s* expr)
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
//evaluates based on the op combo
//NOT called within expressions after the operand <----- FIX ME

ExpressionType CompilerExpression::EvaluateExpressionType(const std::string_view& Operator)
{
	size_t pos = Operator.find('=');
	if (pos != std::string_view::npos) {//has =

		if (Operator.size() == 1) { // = is the only op
			return ExpressionType::EXPR_ASSIGNMENT;
		}
		if (Operator[pos - 1] == '+' ||		//	+=
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
bool CompilerExpression::NextOperatorIsLegal(char previous_op, char op)
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
		return op == '=' || op == '>' || (op == '-');	//	>= || >> || >-
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

	CompilerError("NextOperatorIsLegal: unknown operator [", previous_op, op, "]");
	//int a = 2^-6;

	return false;
}

//takes the raw expression as input
bool CompilerExpression::ParseExpression(std::string& expr)
{

	TokenizeExpression(expr, &e.expression);

	e.expression.type = EvaluateExpressionType(e.expression.Operator);
	e.operands = e.expression.Operator.size();

	switch (e.expression.type) {
	case ExpressionType::EXPR_ASSIGNMENT:
	{
		//fix the contents before the assignment operator
		const std::string variableName = RemoveDuplicateBlanks(RemoveBlanksFromBeginningAndEnd(e.expression.preOP));

		fscript += variableName;
		fscript += e.expression.Operator;
		if (IsVariableInitialization(variableName)) {


			//check the initialization type
			const std::string varType = GetVariableTypeString(variableName);

			if (varType.empty()) {

				CompilerError("Explicit type is missing from variable \"", variableName, "\"");
				return false;

			}

			const size_t type = GetDataType(varType);

			if (!type) {
				CompilerError("Identifier '", varType, "' is unidentified");
				return false;
			}


			

			var.type = (VarType)type;
			var.name = variableName.substr(varType.length() + 1); //the string after the type

			if (!VariableNameIsLegal(var.name)) {
				//CompilerError("Expected an identifier");
				return false;
			}

			if (VariableInStack(var.name)) {
				CompilerError("'", var.name, "' is already defined");
				return false;
			}
		}

		else { //no type specifier
			
			if (ValidNumber(variableName)) { //trying to assign a value to a number
				CompilerError("oh yes, the expression: \"", e.expression.preOP, e.expression.Operator, e.expression.postOP, "\" makes perfect sense!");
				return false;
			}

			//undefined variable
			if (!VariableInStack(variableName)) {
				CompilerError("'", variableName, "' is undefined");
				return false;
			}

			var = *FindVariableFromStack(variableName);
		}

		
	

		std::cout << "variableName: [" << var.name << "]\n";



		expr = CleanupExpression(e.expression.postOP);
		break;
	}
	case ExpressionType::EXPR_CALCULATION:

		expr = CleanupExpression(expr);
		break;

	}
	

	fscript += expr;
	//called AFTER parsing the expression
	ConvertVariablesToValues(expr);
	ParseExpressionNumbers(expr);



	if (!var.name.empty()) {

		scriptStack.PushToStack(var);
		std::cout << std::format("[{}] with type: [{}]\n", var.name, VarTypes[(int)var.type]);
		var.name = "";

	}

	return true;
}
bool CompilerExpression::ConvertVariablesToValues(const std::string_view& expr)
{

}
//variables and functions are not allowed here
//only expects operands, parenthesis and numbers
//an expression like: 101 * (100 + ((2 + 3) & 3 ^ 4)) * (2 | 4 * -(-2 + 3 / (30 - 2 | 3)) ^ 2) can be used here
bool CompilerExpression::ParseExpressionNumbers(std::string& expr)
{

	if (expr.empty()) {
		CompilerError("expected an expression");
		return false;
	}

	//1. find parentheses																			-> 2 * (2 + 3)
	//2. call EvaluateExpression with the expression within the parentheses							-> 2 + 3
	//3. convert the original parenthesis expression to the return value from EvaluateExpression	-> (2 + 3) converts to 5
	//4. resulting string would be:																	-> 2 * 5

	const Parenthesis_s par = GetStringWithinParentheses(expr);

	if (par.count_opening != par.count_closing) {
		CompilerError("mismatching parenthesis");
		return 0;
	}
	if (!par.result_string.empty()) {


		const float result = EvaluateExpression(par.result_string);


		//replace the old expression with the new result
		expr.erase(par.opening, par.strlength + 2);
		expr.insert(par.opening, std::to_string(result));

		ParseExpressionNumbers(expr);



		return 1;
	}

	EvaluateExpression(expr);

	return true;
}

// expects an expression that does not include parantheses
// example: 1 ^ 3 & 1 / 5
bool CompilerExpression::EvaluateExpression(const std::string_view& expression)
{

	if (ValidNumber(expression)) {
		return true;
	}

	std::list<std::string> tokens;
	std::list<expression_stack> expressionstack;


	size_t const opTokens = TokenizeStringOperands(expression, tokens);


	if (opTokens % 2 == 0) {
		CompilerError("EvaluateExpression: Expected an expression");
		return false;
	}



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
bool CompilerExpression::EvaluateExpressionStack(std::list<expression_stack>& es)
{

	const auto L_TestVariableForType = [](std::string& content) -> VarType {
	
		char prefix = content[0];

		if (prefix == '-' || prefix == '+') //variable has a - or + prefix
			content.erase(0, 1);

		Variable* v = FindVariableFromStack(content);

		content.insert(content.begin(), prefix);

		if (!v) {
			CompilerError("'", content, "' is undefined");
			return VarType::VT_INVALID;
		}

		return v->type;
	
	};

	const auto begin = es.begin();
	std::list<expression_stack>::iterator end = es.end();

	std::advance(end, -1); //ignore last because the operator is ""
	VarType leftop{}, rightop{};
	OperatorPriority opriority;
	int32_t iterated = 0;
	bool negative = false;

	for (auto i = begin; i != end; i++) {

		leftop = GetOperandType(i->content);

		if (leftop == VarType::VT_INVALID)  //either a variable or bad syntax
			leftop = L_TestVariableForType(i->content);
		

		//iterate forward to get the right side
		std::advance(i, 1);
		rightop = GetOperandType(i->content);

		if (rightop == VarType::VT_INVALID)  //either a variable or bad syntax
			rightop = L_TestVariableForType(i->content);

		//go back to the left side
		std::advance(i, -1);

		opriority = GetOperandPriority(i->Operator);

		//a string and a non-string
		//is not || or &&

		if ((leftop == VarType::VT_STRING && rightop != VarType::VT_STRING || rightop == VarType::VT_STRING && leftop != VarType::VT_STRING) && (opriority != LOGICAL_AND && opriority != LOGICAL_OR)) {
			CompilerError(VarTypes[(int)leftop], " ", i->Operator, " ", VarTypes[(int)rightop], " is illegal");
			return false;
		}

		//both are strings
		//equality operator not used

		else if ((leftop == VarType::VT_STRING && rightop == VarType::VT_STRING) && (opriority != LOGICAL_AND && opriority != LOGICAL_OR && opriority != EQUALITY)) {
			CompilerError(VarTypes[(int)leftop], " ", i->Operator, " ", VarTypes[(int)rightop], " is illegal");
			return false;
		}

		OperatorPriority a = GetOperandPriority(i->Operator); //errors if there are bad operators

		if (a == FAILURE)
			return false;

		++iterated;


	}
	if (end->Operator != "" || !iterated) {

		if (!iterated) {
			if (GetOperandType(es.front().content) == VarType::VT_INVALID) {
				CompilerError("EvaluateExpressionStack: Expected an expression\n");
				return false;
			}
		}


		CompilerError("EvaluateExpressionStack: Expected an expression\n");
		return false;
	}
	return true;
}
VarType CompilerExpression::GetOperandType(const std::string_view& operand)
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

	return VarType::VT_INVALID; //could still be a variable

	//Variable* v = FindVariableFromStack(operand);

	//if (!v) {
	//	CompilerError("'", operand, "' is undefined");
	//	return VarType::VT_INVALID;
	//}

	//return (VarType)v->type;
}