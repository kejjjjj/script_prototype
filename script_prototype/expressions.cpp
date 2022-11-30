#include "pch.h"

//function expects a string: 
//a <operand> b
ExpressionType_e Expression::GetExpressionType()
{
	size_t const assigntokens = GetStringTokens(expression_str, '=');

	if (assigntokens == 1)
		return ExpressionType_e::EXPR_ASSIGNMENT; //a = 1

	else if(assigntokens == 2)
		return ExpressionType_e::EXPR_COMPARE; // a == 1

	return ExpressionType_e::EXPR_CALCULATION; // a + 1
}

//remove all whitespaces
//keep all quoted strings
std::string Expression::RemoveWhiteSpaces(std::string& expr)
{

	//parsing is easier when there are no unnecessary characters
	std::string converted_str, temp;
	size_t tokens = GetStringTokens(expr, '"');

	//this would never be valid syntax unless it is \"
	if (tokens % 2 != 0)  CompilerError(std::format("Odd number of quotation marks in string:\n {}", expr));
	
	//no quotation marks so remove all whitespaces
	if (tokens == NULL) converted_str = RemoveBlank(expr);

	//string has quotation marks so remove everything outside of them
	else {
		std::vector<std::string> strTok;
		TokenizeString(expr, '"', strTok);
		std::cout << '\n';
		int j = 0;
		for (const auto& i : strTok) {



			if (j % 2 == 0 /*|| (j == strTok.size() - 1u)*/) { //not within quotations
				temp.clear();
				RemoveBlank(strTok[j], temp);

				
				converted_str.resize(converted_str.size() + temp.size());
				converted_str += temp;
				j++;

			}
			else {
				
				converted_str.resize(converted_str.size() + strTok[j].size() + 2);
				converted_str += '"';
				converted_str += strTok[j];
				converted_str += '"';
				j++;
			}
			//std::cout << std::format("[{}]: {}\n", j++, i);
			


		}

	}

	return converted_str;
}


//expects a full string formatted like:
// a <operand> b
void Expression::TokenizeExpression(expression_s* expr)
{

	bool firstOperand = FALSE;
	bool Operand_processed = FALSE; //true after parsing the expression operators 
	std::string token;
	for (auto& i : expression_str) {

		if (!Operand_processed) {

			const bool isOperandCharacter = (i == '+' || i == '-' || i == '*' || i == '/' || i == '=');

			//first operand
			if (!firstOperand && isOperandCharacter) {
				expr->Operand.push_back(i);
				firstOperand = true;
				expr->preOP = token; //store left side
				token.clear();
				continue;
			}

			//second operand
			else if (firstOperand && isOperandCharacter) { //2 operators (+= , /=)
				expr->Operand.push_back(i);
				Operand_processed = true;
				continue;
			}
			if(firstOperand)
				Operand_processed = true;
		}

		token.push_back(i);
	}
	expr->postOP = token;

	if (expr->preOP.empty() || expr->postOP.empty() || expr->Operand.empty()) {
		CompilerError("Expected an expression");
	}
}
//returns false if there are syntax errors
//removes unnecessary blank characters from the strings
bool Expression::AssignmentIsSane(expression_s* expr)
{
	/*TODO
		logic for functions some day... if ever
	*/



	//pre-operand rules:
	//1. first character cannot be a number
	//2. only alphanumerical characters 

	//remove all blanks before and after the string
	expr->preOP = RemoveBlanksFromBeginningAndEnd(expr->preOP);

	//1. first character cannot be a number
	if (std::isdigit(expr->preOP[0])) {
		CompilerError("Expected an identifier");
		return false;
	}


	//2. only alphanumerical characters
	for (auto& i : expr->preOP) {
		if (!std::isalnum(i)) {
			CompilerError("Expected an identifier");
			return false;
		}
	}

	//operand rules:
	//1. maximum of 2 characters
	//2. is operator character (already checked in TokenizeExpression)
	if (expr->Operand.size() > 2) {
		CompilerError("Too many operators in expression");
		return false;
	}

	//post-operand rules
	//1. allow everything if there are two quotation marks
	//2. if there are more than 2 quotation marks, fail

	//remove all blanks before and after the string
	expr->postOP = RemoveBlanksFromBeginningAndEnd(expr->postOP);

	size_t const quotationmarks = GetStringTokens(expr->postOP, '"');

	if (quotationmarks > 0) { //string contains quotation marks

		if (quotationmarks > 2) { //not allowed yet
			CompilerError("Too many quotation marks post-operand");
			return false;
		}

		//first and last characters have to be quotation marks
		if (expr->postOP[0] != '"' || expr->postOP[expr->postOP.size() - 1] != '"') {
			CompilerError("Illegal usage of quotation marks post-operand");
			return false;
		}
	}
	//there are no quotation marks if this is else statement executes
	else {

		//1. first character cannot be a number
		if (std::isdigit(expr->postOP[0])) {
			//test if all characters are numbers
			for (auto& i : expr->postOP) {

				//error if there is non-digit
				if (!std::isdigit(i)) {
					CompilerError("Expected an identifier");
					return false;
				}
			}

		}


		//2. only alphanumerical characters
		for (auto& i : expr->preOP) {
			if (!std::isalnum(i)) {
				CompilerError("Expected an identifier");
				return false;
			}
		}
	}


	std::cout << std::format("preOP: {}\nOperand: {}\npostOP: {}\n", expr->preOP, expr->Operand, expr->postOP);

}
bool Expression::EvaluateExpression()
{

	expression_s expression;

	expression.type = GetExpressionType();
	TokenizeExpression(&expression);

	if (!AssignmentIsSane(&expression))
		return false;



	return true;
}