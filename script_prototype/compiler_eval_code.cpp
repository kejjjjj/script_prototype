#include "pch.h"
bool cec::Compiler_SemiColon(const token_t* token)
{
	if (token->value == ";") {

		if (syntax.parantheses.count)
			CompilerError("Expected a \")\"");

		syntax.CheckRules(S_EXPRESSION);
		return true;
	}
	return false;
}
void cec::Compiler_Parantheses(const token_t* token)
{
	if (token->t_type != token_t::tokentype::OTHER || token->value.size() > 1)
		return;

	auto front = token->value.front();

	if (front == '(') {
		syntax.CheckRules(S_SEMICOLON);
		syntax.AddFlag(S_EXPRESSION);
		syntax.ClearFlag(S_SEMICOLON);
		syntax.parantheses.count++;
	}
	else if (front == ')') {

		if (!syntax.parantheses.count) { //epic error!
			syntax.AddFlag(S_SEMICOLON);
			syntax.CheckRules(S_SEMICOLON);
		}
		syntax.parantheses.count--;
	}
}
bool cec::Compiler_WhiteSpace(const token_t* token)
{

	if (!std::isspace(token->whitespace))
		return false;

	return true;
}

token_t cec::Compiler_ReadToken(std::string::iterator& it)
{


	token_t token; //entire string that can include whitespaces
	bool isspace, isalnum;
	char ch;

	ch = *it;
	isspace = std::isspace(ch);
	isalnum = std::isalnum(ch);

	if (std::isdigit(ch)) {
		token.t_type = token_t::tokentype::DIGIT;
		token.eval_fc = std::make_unique<std::function<bool( token_t*)>>(Compiler_NumericToken);
	}
	else if (IsAnyOperator(ch)) {
		token.t_type = token_t::tokentype::OPERATOR;
		token.eval_fc = std::make_unique<std::function<bool( token_t*)>>(Compiler_OperatorToken);

	}
	else if (std::isalpha(ch)) {
		token.t_type = token_t::tokentype::STRING;
		token.eval_fc = std::make_unique<std::function<bool( token_t*)>>(Compiler_StringToken);
	}
	else if (std::isspace(ch)) { //store whitespaces to maintain 1:1 to original code
		++it;
		token.t_type = token_t::tokentype::OTHER;
		token.whitespace = ch;
		return token;
	}
	else {
		++it;
		token.t_type = token_t::tokentype::OTHER;
		token.value.push_back(ch);
		return token;
	}
	//if (t_type == INVALID) {
	//	CompilerError("Unrecognized token type: \"", ch, "\"");
	//	return std::nullopt;
	//}
	token.value.push_back(ch);
	ch = *(++it);

	while (it != f_str.end()) {
		switch (token.t_type) {
		case token_t::tokentype::DIGIT:
			if (!std::isdigit(ch)) {
				return token;
			}
			break;
		case token_t::tokentype::OPERATOR:
			if (!IsAnyOperator(ch)) {
				return token;
			}
			break;
		case token_t::tokentype::STRING:
			if (!std::isalnum(ch)) {
				return token;
			}
			break;
		default:
			CompilerError("Compiler_ReadToken(): default case");
			break;


		}
		token.value.push_back(ch);
		ch = *(++it);
		

	}
	if(token.value != ";")
		CompilerError("Compiler_ReadToken(): unexpected end of file");

	return token;
}

//only checks for syntax errors
//ignores the stack completely, so this will happily return uninitialized variables/functions
//this function is recursive, so stack overflows are possible if there are REALLY long expressions
code_type cec::Compiler_ReadNextCode3(std::string::iterator& it)
{
	code_type code;
	token_t token;

	//Sleep(500);

	token = Compiler_ReadToken(it);

	if (Compiler_SemiColon(&token)) {
		return code;
	}
	std::cout << "token: " << token.value << '\n';

	if (!Compiler_WhiteSpace(&token)) {
		Compiler_Parantheses(&token);
		if (token.eval_fc.get()) {
			if (!(*token.eval_fc.get())(&token)) { //call the assigned function
				CompilerError("Syntax Error!");
				return code;
			}
		}
		else
			std::cout << "no assigned function\n";
	}
	else { //whitespaces are unnecessary in most cases, so don't include them
		token.value.clear();
	}
	code.code = token.value + Compiler_ReadNextCode3(it).code;

	
	

	//CompilerError("Compiler_ReadNextCode(): unexpected end of file");
	return code;
}
bool cec::Compiler_NumericToken(const token_t* token)
{
	if (!token)
		return false;

	syntax.CheckRules(S_SEMICOLON);

	syntax.ClearFlag(S_EXPRESSION);


	syntax.AddFlag(S_SEMICOLON);

	return true;
}
bool cec::Compiler_StringToken(const token_t* token)
{
	if (!token)
		return false;

	syntax.CheckRules(S_SEMICOLON);

	syntax.ClearFlag(S_EXPRESSION);

	syntax.AddFlag(S_SEMICOLON);

	return true;
}
bool cec::Compiler_OperatorToken(token_t* token)
{
	if (!token)
		return false;

	std::optional<std::list<std::string>> tokens_opt;

	if (!(tokens_opt = TokenizeOperatorSequence(token->value))) {
		return false;
	}

	//tokens contains a list of tokenized operators
	const auto& tokens = tokens_opt.value(); 
	token->value.clear();
	bool p_ws = false;
	for (auto& i : tokens) {
		std::cout << "op: " << i << '\n';
		token->value += Compiler_SyntaxCheckOperator(i, p_ws); //fix the whitespaces
	}
	return true;
	int a = 1;
	//1+++a;
}
std::string cec::Compiler_SyntaxCheckOperator(const std::string& op, bool& p_ws)
{
	const auto front = op.front();
	if (IsUnaryOperator(op)) {
		if(front == '~' || front == '!')
			syntax.CheckRules(S_SEMICOLON);  //unary ~ and ! cannot be used after an expression

		else if (syntax.FlagActive(S_SEMICOLON) && UnaryArithmeticOp(op)) { //we just read a numeric/string token and this is a postfix operator
			return p_ws = true, (op + ' ');
		}
		else if (UnaryArithmeticOp(op)) {
			return p_ws ? op : (' ' + op); //fixes a duplicate whitespace
		}
	}
	else {
		syntax.CheckRules(S_EXPRESSION); //not an unary operator, so error out if we are expecting an expression, example: a - / 1;

	}

	if (syntax.FlagActive(S_SEMICOLON)) { //we just read a numeric/string token and this is a valid operator, so expect an expression next and clear semicolon
		syntax.AddFlag(S_EXPRESSION); //expecting an expression because the latest token is an operator
		syntax.ClearFlag(S_SEMICOLON);
	}

	return op;
}