#include "pch.h"

//1. check for space-dependent syntax errors before cleaning the expression
void CompilerExpression::TestExpression(const std::string_view& expr)
{
	char last_character{ '\0' };
	int32_t operands_in_a_row{ 0 };
	bool within_quotes = false,
		blank_will_fail = false, //if the next character is blank, syntax error. | example: int a = 2 3;
		bAlnum = false,
		space_will_fail = false;

	int32_t it_since_quote = 0;

	auto begin = expr.begin();
	auto end = expr.end();



	for (auto it = expr.begin(); it != end; it++) {
		char i = *it;

		bAlnum = std::isalnum(i);

		if (!bAlnum && BadCalculationOp(i) && i != '\n' && i != '"' && i != '_' && !std::isspace(i) && !within_quotes) {

			if (i != expr.front()) { //is . used because of a decimal point?
				if (i == '.' && std::isdigit(*(it - 1))) { //if so, then skip
					goto isfine;
				}
			}

			CompilerError("Illegal character '", i, "' used in expression");
			return;
		}
	isfine:

		if (IsCalculationOp(i)) {
			if (operands_in_a_row > 2) {
				CompilerError("Illegal amount of expression operators");
				return;
			}
			if (!NextOperatorIsLegal(last_character, i)) {
				CompilerError("Illegal operator sequence '", last_character, i, "'");
				return;
			}

			last_character = i;
			operands_in_a_row++;
			//continue;
		}
		else {
			operands_in_a_row = false;
			last_character = '\0';
		}
		//int a = 2 3;

		if (space_will_fail && std::isspace(i)) {
			CompilerError("expected a ';'");
			return;
		}


		if (!within_quotes && blank_will_fail){
			
			if (std::isspace(i)
				|| *(it - 1) == ')' //no operator after a closing parenthesis
				|| *(it - 1) == '"' //no operator after closing quote
				|| *it == '"' && std::isalnum(*(it - 1)) && *(it - 1) != '_') { //no operator after number/variable and the next character is a quote

				while (std::isspace(*it)) { 
					it++; }

				if (!IsCalculationOp(*it) && i != ';' && *it != ')') {
					CompilerError("expected a ';'");
					return;
				}
			}

		}

		if (i == '"')
			within_quotes = !within_quotes;

		blank_will_fail = (bAlnum || i == '"' || i == '_' || i ==')');
		space_will_fail = i == '.';
	}
}

//1. remove blanks
//returns "" if it fails
std::string CompilerExpression::CleanupExpression(const std::string_view& expr)
{

	char last_character{ '\0' };
	int32_t operands_in_a_row{ 0 }, idx{ -1 };
	bool within_quotes = false,
		bAlnum = false;

	for (const auto& i : expr) {
		idx++;

		//pre-, and post-increments here probably...
		// because ++ != + +

		//idk I will probably do this when I actually have variable support

		if (i == '"') 
			within_quotes = !within_quotes;
		

		if (std::isspace(i))
			continue;

		bAlnum = std::isalnum(i);

		if (!bAlnum && BadCalculationOp(i) && i != '\n' && i != '"' && i != '_' && !within_quotes) {

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
				//FIXME

				//the ! prefix after an operator

				//FIXME
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

	}

	if (expr->preOP.empty())
		expr->preOP = token;

	else
		expr->postOP = token;
}
//evaluates based on the op combo
//NOT called within expressions after the operand <----- FIX ME

ExpressionType CompilerExpression::EvaluateExpressionType(expression_s* expr)
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

			return ExpressionType::EXPR_ASSIGNMENT2;

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
	std::string variableName;

	TokenizeExpression(expr, &e.expression);

	e.expression.type = EvaluateExpressionType(&e.expression);
	e.operands = e.expression.Operator.size();

	switch (e.expression.type) {
	case ExpressionType::EXPR_ASSIGNMENT:
	{
		//fix the contents before the assignment operator
		variableName = RemoveDuplicateBlanks(RemoveBlanksFromBeginningAndEnd(e.expression.preOP));

		fscript += variableName;
		fscript += e.expression.Operator;
		if (IsVariableInitialization(variableName)) {

			if (e.expression.Operator.empty()) {
				CompilerError("Uninitialized variable: \"", variableName, "\"");
				return false;
			}

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

			if (!var.name.empty()) {

				scriptStack.PushToStack(var);
				std::cout << std::format("pushing [{}] to stack with type: [{}]\n", var.name, VarTypes[(int)var.type]);
				/*var.name = "";*/

			}
			variableName = var.name;
		}

		else { //no type specifier
			
			if (ValidNumber(variableName)) { //trying to assign a value to a number
				CompilerError("oh yes, the expression: \"", e.expression.preOP, e.expression.Operator, e.expression.postOP, "\" makes perfect sense!");
				return false;
			}

			if (IsDataType(variableName)) {
				CompilerError("type name is not allowed");
				return false;
			}

			if (!VariableInStack(variableName)) {
				CompilerError("'", variableName, "' is undefined");
				return false;
			}

			
		}

		var = *FindVariableFromStack(variableName);
	

		std::cout << "variableName: [" << var.name << "]\n";

		TestExpression(e.expression.postOP);
		expr = CleanupExpression(e.expression.postOP);
		break;
	}
	case ExpressionType::EXPR_ASSIGNMENT2:

		variableName = RemoveDuplicateBlanks(RemoveBlanksFromBeginningAndEnd(e.expression.preOP));

		fscript += variableName;
		fscript += e.expression.Operator;
		TestExpression(expr);
		expr = CleanupExpression(e.expression.postOP);

		if (!FindVariableFromStack(variableName)) {
			CompilerError("undefined variable '", variableName, "'");
			return false;
		}

		//CompilerError("NIGGER");

		break;
	case ExpressionType::EXPR_CALCULATION:
		TestExpression(e.expression.postOP);
		expr = CleanupExpression(expr);
		break;
	default:
		CompilerError("impossible");
		return false;

	}
	

	fscript += expr;

	//called AFTER parsing the expression
	ParseExpressionNumbers(expr);




	return true;
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

	std::cout << "CompilerExpression: " << expression << '\n';

	if (opTokens % 2 == 0) {
		CompilerError("EvaluateExpression: Expected an expression");
		return false;
	}

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
bool CompilerExpression::EvaluateSingular(std::string& content)
{
	auto variable_prefix = HasPrefix(content);
	bool fix = !variable_prefix.empty();

	if (fix) //variable has a - or + prefix
		content.erase(0, variable_prefix.size());

	const Variable* v = FindVariableFromStack(content);


	if (!v) {

		if (IsDataType(content)) {
			CompilerError("type name is not allowed");
			return false;
		}
		if (!ValidNumber(content)) {
			CompilerError("variable '", content, "' is undefined");
			return false;
		}
		//if (fix)
		//	for (auto& i : variable_prefix)
		//		content.insert(content.begin(), i);

		return true; //is a valid number lol pog

		//return false;
	}

	//if (fix)
	//	for (auto& i : variable_prefix)
	//		content.insert(content.begin(), i);



	return true;




}
//variable names can be passed
bool CompilerExpression::EvaluateExpressionStack(std::list<expression_stack>& es)
{

	const auto L_TestVariableForType = [](std::string& content) -> VarType {
	
		auto variable_prefix = HasPrefix(content);
		bool fix = !variable_prefix.empty();

		if (fix) //variable has a - or + prefix
			content.erase(0, variable_prefix.size());

		const Variable* v = FindVariableFromStack(content);


		if (!v) {

			if (IsDataType(content)) {
				CompilerError("type name is not allowed");
			}
			CompilerError("'", content, "' is undefined");

			if (fix)
				for (auto& i : variable_prefix)
					content.insert(content.begin(), i);

			return VarType::VT_INVALID;
		}

		if (fix)
			for (auto& i : variable_prefix)
				content.insert(content.begin(), i);

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
			CompilerError("cannot cast from '", VarTypes[(int)leftop], "' to '", VarTypes[(int)rightop], "'");
			return false;
		}

		//both are strings
		//equality operator not used

		else if ((leftop == VarType::VT_STRING && rightop == VarType::VT_STRING) && (opriority != LOGICAL_AND && opriority != LOGICAL_OR && opriority != EQUALITY && i->Operator != "+")) {
			CompilerError("cannot cast from '", VarTypes[(int)leftop], "' to '", VarTypes[(int)rightop], "'");
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

				if (L_TestVariableForType(es.front().content) == VarType::VT_INVALID) {

					CompilerError("unrecognized token: '", es.front().content, "'");
					return false;
				}
			}
		}

		else {
			CompilerError("EvaluateExpressionStack: Expected an expression\n");
			return false;
		}
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

	//return VarType::VT_INVALID; //could still be a variable

	Variable* v = FindVariableFromStack(operand);

	if (!v) {
		//CompilerError("'", operand, "' is undefined");
		return VarType::VT_INVALID;
	}

	return (VarType)v->type;
}