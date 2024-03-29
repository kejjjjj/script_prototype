#include "pch.h"

expr::expression_token expr::EvaluateEntireExpression(const std::string& str)
{

	srules.typename_allowed = false;

	if (str.empty()) {
		std::cout << "EvaluateEntireExpression(): empty expression\n";
		return {};
	}

	Substr_s par = ExpressionFindParentheses(const_cast<std::string&>(str));

	if (par.result_string == "empty") {
		par.result_string = "";
	}

	//parentheses exists
	if (!par.result_string.empty()) {
		std::string copy = str;

		auto result = EvaluateExpression(par.result_string);

		copy.erase(par.opening, par.strlength + 2ull);
		copy.insert(par.opening, ' ' + result.content + ' '); //spacebar fixes cases like (500+200)0 (without the space this would turn into 7000)
		

		result = EvaluateEntireExpression(copy);
		return result;
	}
	auto result = expr::EvaluateExpression(str);
	return result;
}
expr::expression_token expr::EvaluateExpression(const std::string& str)
{
	rules.next_postfix = false;
	rules.next_unary = true;
	rules.next_operator = false;
	srules.expecting_identifier = false;

	syntax.ClearFlag(S_END_OF_NUMBER);
	
	std::cout << "EvaluateExpression(" << str << ")\n";
	std::string s_str = std::string(str);
	auto it = s_str.begin(); auto end = s_str.end();
	std::list<expression_token> tokens;
	TokenizeExpression(it, end, tokens);
	

	std::for_each(tokens.begin(), tokens.end(), SetTokenValueCategory);
	auto tbegin = tokens.begin(); auto tend = tokens.end();
	EvaluatePostfix(tbegin, tend, tokens); tbegin = tokens.begin();
	syntax.ClearFlag(S_END_OF_NUMBER);
	EvaluatePrefix(tbegin, tend);
	return EvaluateExpressionTokens(tokens);

}
Substr_s expr::ExpressionFindParentheses(std::string& expression)
{
	auto it = expression.begin();
	auto end = expression.end();

	size_t idx = 0;
	size_t opening{ 0 }, closing{ 0 }, count_opening{ 0 }, count_closing{ 0 };

	while (it != end) {
		const token_t token = cec::Compiler_ReadToken(it, '\0', end);

		if (token.t_type == token_t::tokentype::PARENTHESIS) {
			if (token.value == "(") {
				count_opening = 1;
				opening = idx;
			}
			else {
				count_closing = 1;
				closing = idx;
			}

			if (count_opening > 0 && count_opening == count_closing) {
				break;
			}

		}

		if (it == end && count_opening == 1)
			throw std::exception("expected a \")\"");
			

		idx += token.value.size();

	}

	const size_t len = closing - opening - count_opening;
	//note: cuts out the s and e characters
	std::string result_string = std::string(expression).substr(opening + 1, len);
	if (count_opening && count_closing && result_string.empty())
		result_string = ("empty");

	return { count_opening, count_closing, opening, len, result_string };


}
void expr::TokenizeExpression(std::string::iterator& it, std::string::iterator& end, std::list<expression_token>& tokens, int tokens_left)
{
	expression_token expr_token;

	if (!tokens_left)
		return;

	bool kill_loop = false;
	rules.next_unary = true;
	rules.next_postfix = false;
	rules.ignore_postfix = false;
	rules.operator_allowed = true;
	rules.suffix_allowed = false;

	if (!tokens.empty()) {
		if (tokens.back().postfix.front() == ".") { //for cases like 2. + 1;
			rules.next_unary = false;
		}
	}
	while (!kill_loop && it != end) {

		const token_t token = cec::Compiler_ReadToken(it, '\0', end);
		
		if (token.t_type == token_t::tokentype::STRING_LITERAL) {
			expr_token.string_literal = true;
		}
		else if (token.t_type == token_t::tokentype::CHAR_LITERAL) {
			expr_token.char_literal = true;
			expr_token.tokentype = VarType::VT_CHAR;
		}



		switch (token.t_type) {
		case token_t::tokentype::STRING_LITERAL:
		case token_t::tokentype::CHAR_LITERAL:
		case token_t::tokentype::STRING:
		case token_t::tokentype::DIGIT:

			if (expr_token.tokentype == VarType::VT_INVALID) {
				if(token.t_type == token_t::tokentype::STRING || token.t_type == token_t::tokentype::STRING_LITERAL)
					expr_token.tokentype = VarType::VT_STRING;
				else
					expr_token.tokentype = VarType::VT_INT;
			}

			if (rules.suffix_allowed && token.t_type == token_t::tokentype::STRING) {
				auto suffix = literal_suffix.find(token.value);
				if (suffix != literal_suffix.end()) {
					expr_token.tokentype = suffix->second;
					std::cout << "suffix of type: " << VarTypes[int(suffix->second)] << '\n';

					kill_loop = true;
					break;
				}
				throw std::exception(std::format("unrecognized suffix \"{}\"", token.value).c_str());
				
			}
			rules.suffix_allowed = false;

			if (token.t_type == token_t::tokentype::DIGIT)
				rules.suffix_allowed = true;

			rules.operator_allowed = true;

			if (rules.next_operator) {
				throw std::exception("expected a \";\"");
			}
			if (!rules.next_unary && rules.next_postfix) {
				it -= token.value.length();
				kill_loop = true;
				break;
			}
			rules.next_unary = false;
			rules.next_postfix = true;
			expr_token.content = token.value;
			if (rules.ignore_postfix) {
				kill_loop = true;
				break;
			}

			break;
		case token_t::tokentype::OPERATOR:

			if (!rules.operator_allowed) {
				throw std::exception("expected an integral type instead of operator");
			}
			
			rules.ignore_postfix = false;
			rules.suffix_allowed = false;
			if (rules.next_operator) {
				expr_token.op = true;
				if(!SatisfiesOperator(token.value))
					throw std::exception("expected an expression");

				expr_token.content = token.value;
				kill_loop = true;
				break;
			}
			if ((IsUnaryOperator(token.value) || token.value == ".") && (rules.next_unary)) {
				expr_token.prefix.push_back(token.value);

				if (token.value == ".") { // a decimal number that begins with .
					rules.ignore_postfix = true;
					rules.operator_allowed = false;
				}

			}
			else if (IsPostfixOperator(token.value)) {

				if(!rules.next_postfix)
					throw std::exception(std::format("unexpected operator {}", token.value).c_str());

				if (token.value == "[") {
					auto full_expr = std::string(it, end);
					auto iend = full_expr.find_first_of(']');

					if (iend == std::string::npos) {
						throw std::exception("expected a \"]\"");
					}
					
					

					expr_token.postfix.push_back('[' + full_expr.substr(0, iend + 1));

					it += iend+1;

				}

				else if (rules.next_postfix && !UnaryArithmeticOp(token.value)) {
					expr_token.postfix.push_back(token.value); //just a postfix
				}



			}else if (token.value.front() == '.' && rules.next_postfix) {

				expr_token.postfix.push_back(token.value);
				kill_loop = true;
				rules.next_operator = true;
				break;
			}

			else {
				it -= token.value.length();
				kill_loop = true;
			}

			break;
		case token_t::tokentype::WHITESPACE:
			rules.suffix_allowed = false;
			if (expr_token.content.empty())
				expr_token.whitespace = true;
			break;
		}
	}
	rules.next_operator = !rules.next_operator;
	if (!expr_token.content.empty()) { //ignore empty tokens
		expr_token.whitespace = false;
		tokens.push_back(expr_token);
	}
	if(it != end)
		TokenizeExpression(it, end, tokens, tokens_left != -1 ? --tokens_left : -1);

	

	return;
}
void expr::SetTokenValueCategory(expression_token& token)
{


	if (token.op)
		return;



	if (token.tokentype != VarType::VT_STRING || token.string_literal) {

		token.rval = std::shared_ptr<rvalue>(new rvalue(token.tokentype, token.string_literal ? token.content.size() : 0));
		switch (token.get_type()) {
		case VarType::VT_INT:
			token.rval->set_value<int>(std::stoi(token.content));
			break;
		case VarType::VT_FLOAT:
			token.rval->set_value<float>(std::stof(token.content));
			break;
		case VarType::VT_STRING:
			token.rval->set_string((char*)token.content.c_str());
			std::cout << "the str: " << token.rval->get_string() << '\n';
			break;
		case VarType::VT_CHAR:

			if (std::distance(token.content.begin(), token.content.end()) < 2 && token.char_literal) {
				throw std::exception("what the hell?");
			}

			//std::cout << "literal: " << token.char_literal << '\n';
			int32_t value = token.char_literal ? std::accumulate(++token.content.begin(), --token.content.end(), 0) : std::stoi(token.content);

			//if (str_len >= 127) {
			//	throw std::exception("char literal exceeds the value 127");
			//	return;
			//}

			token.rval->set_value<char>(static_cast<char>(value));
			break;
		}
		return;
	}

	if (IsDataType(token.content))
		throw std::exception("data type is not allowed");

	const auto v = stack_variables.find(token.content);
	if (v == stack_variables.end()) {
		throw std::exception(std::format("identifier \"{}\" is undefined", token.content).c_str());
	}

	token.lval = std::shared_ptr<lvalue>(new lvalue);
	token.lval->ref = &v->second;

	token.set_type(v->second.get_type());
}
void expr::EvaluatePostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens)
{
	if (it == end) {
		if ((--it)->op) {
			throw std::exception("expected an expression");
		}
		++it;
		return;
	}

	auto& token = *it;

	if (token.op)
		syntax.ClearFlag(S_END_OF_NUMBER);

	if (token.content.empty())
		throw std::exception("expected an operand");

	if (token.postfix.empty())
		return EvaluatePostfix(++it, end, tokens);

	if (EvaluateSubscript(*it)) {
		token.postfix.pop_front();
		return EvaluatePostfix(it, end, tokens);
	}

	if (UnaryArithmeticOp(token.postfix.front())) {
		if (token.is_rvalue())
			throw std::exception("increment/decrement operand must be an lvalue");

		EvaluatePostfixArithmetic(token, token.postfix.front() == "++");
		token.postfix.pop_front();
		return EvaluatePostfix(it, end, tokens);
	}

	if(EvaluatePeriodPostfix(it, end, tokens))
		return EvaluatePostfix(it, end, tokens);

	//this will NOT execute until variable support
	token.postfix.pop_front();
	syntax.ClearFlag(S_END_OF_NUMBER);
	
	return EvaluatePostfix(it, end, tokens);
}
void expr::EvaluatePrefix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end)
{
	if (it == end)
		return;

	auto& token = *it;

	if (token.op)
		syntax.ClearFlag(S_END_OF_NUMBER);

	if (token.content.empty())
		throw std::exception("expected an operand");

	if (token.prefix.empty()) {
		return EvaluatePrefix(++it, end);
	}

	if (EvaluatePeriodPrefix(it))
		return EvaluatePrefix(it, end);

	if (EvaluateAddressOfPrefix(*it)) {
		token.prefix.pop_back();
		return EvaluatePrefix(it, end);

	}

	if (UnaryArithmeticOp(token.prefix.back())) {

		EvaluatePrefixArithmetic(token, token.prefix.back() == "++");
		token.prefix.pop_back();
		return EvaluatePrefix(it, end);
	}

	expression_token r_operand;
	r_operand.rval = std::shared_ptr<rvalue>(new rvalue(VarType::VT_INT));
	
	switch (token.prefix.back().front())
	{
		case '-':
			ExpressionMakeRvalue(token);

			r_operand.rval->set_value<int>(-1);
			token.content = eval_funcs.find("*")->second(token, r_operand).content;
			break;
		case '+':
			ExpressionMakeRvalue(token);

			r_operand.rval->set_value<int>(1);
			token.content = eval_funcs.find("*")->second(token, r_operand).content;
			break;
		case '~':
			ExpressionMakeRvalue(token);

			if (!token.is_integral())
				throw std::exception("bitwise complement expects an integral operand");

			token.set_value(~token.get_int());
			token.content = std::to_string(token.get_int());

			break;
		case '!':
			ExpressionMakeRvalue(token);

			r_operand.rval->set_value<int>(0);
			token.content = eval_funcs.find("==")->second(token, r_operand).content;

			break;
		default:
			break;
	}

	token.set_type(r_operand.get_type());
	token.set_value<int>(r_operand.get_int());

	token.prefix.pop_back();

	return EvaluatePrefix(it, end);
}
bool expr::EvaluatePeriodPrefix(std::list<expression_token>::iterator& it)
{
	if (it->prefix.back() != ".") {
		return false;
	}
	if (IsInteger(it->content)) {
		if (syntax.FlagActive(S_END_OF_NUMBER))
			syntax.CheckRules(S_END_OF_NUMBER);
		syntax.AddFlag(S_END_OF_NUMBER);

		it->content = "0." + it->content;
		it->set_type(VarType::VT_FLOAT);
		it->rval->set_value<float>(std::stof(it->content));

		it->prefix.pop_back();

		return true;
	}

	throw std::exception("expected an integral type");

	return false;

}
bool expr::EvaluateSubscript(expression_token& token)
{
	const auto& postfix = token.postfix.front();

	if (postfix.front() != '[')
		return false;

	if (!token.is_lvalue())
		throw std::exception("[] operand must be an lvalue");

	if(!token.lval->ref->is_array())
		throw std::exception("[] operand must be have array type");

	const auto result = expr::EvaluateEntireExpression(postfix.substr(1, postfix.size() - 2)); //remove the brackets
	const auto arrSize = token.lval->ref->numElements;

	if (!result.is_integral()) {
		throw std::exception("expression must be convertible to an integral type");
	}



	const int numElements = result.implicit_cast<int>();

	if (numElements < 0 || numElements >= arrSize)
		throw std::exception(std::format("accessing an array out of bounds [tried {} when size was {}]", numElements, arrSize).c_str());

	token.lval->ref = &token.lval->ref->arr[numElements];

	std::cout << "array lvalue -> " << token.lval->ref << '\n';

	return true;
}
bool expr::EvaluateAddressOfPrefix(expression_token& token)
{
	if (token.prefix.back() != "@")
		return false;

	if (!token.is_lvalue())
		throw std::exception("address-of operand must be an lvalue");

	token.rval = std::shared_ptr<rvalue>(new rvalue(token.get_type(), sizeof(void*)));

	token.rval->pointer = std::shared_ptr<Variable>(token.lval->ref);
	token.lval.reset();

	return true;

}
void expr::EvaluatePostfixArithmetic(expression_token& token, bool increment)
{
	if (token.is_rvalue())
		throw std::exception("expression must be an lvalue");

	const auto type = token.lval->ref->get_type();

	if (type != VarType::VT_INT && type != VarType::VT_FLOAT)
		throw std::exception("expected an int or float");

	expression_postfixes.push_back({ token.lval->ref , increment });

	ExpressionMakeRvalue(token);
}
void expr::EvaluatePrefixArithmetic(expression_token& token, bool increment)
{
	if (token.is_rvalue())
		throw std::exception("expression must be an lvalue");

	const auto type = token.lval->ref->get_type();

	if (type != VarType::VT_INT && type != VarType::VT_FLOAT)
		throw std::exception("expected an int or float");
	
	if (type == VarType::VT_FLOAT) {

		token.set_value<float>(increment == true ? token.get_float() + 1 : token.get_float()  -1);
		token.content = std::to_string(token.get_float());

	}
	else {
		token.set_value<int>(increment == true ? token.get_int() + 1 : token.get_int() - 1);
		token.content = std::to_string(token.get_int());
	}
	std::cout << "it's now " << token.content << '\n';
	ExpressionMakeRvalue(token);

}
bool expr::EvaluatePeriodPostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens)
{
	if (it->postfix.front() != ".") {
		return false;
	}
	
	if (IsInteger(it->content)) {
		if(syntax.FlagActive(S_END_OF_NUMBER))
			syntax.CheckRules(S_END_OF_NUMBER);
		syntax.AddFlag(S_END_OF_NUMBER);

		//check what is on the right hand side of the postfix
		
		std::list<expression_token>::iterator it2 = it;
		++it2;

		if (it2 == end || it2->op) {

			it->content += ".0"; //create a floating point value
			it->postfix.pop_front();
			it->set_type(VarType::VT_FLOAT);
			it->rval->set_value<float>(std::stof(it->content));

			return true;
			//tokens.erase(it2);
		}

		if (!it2->whitespace) {
			if (it2->content.empty() || IsInteger(it2->content)) { //floating point literal

				if(!it2->postfix.empty() || !it2->prefix.empty())
					syntax.CheckRules(S_END_OF_NUMBER); //force error

				it->content += "." + (it2->content.empty() ? "0" : it2->content); //create a floating point value
				it->postfix.pop_front();
				it->set_type(VarType::VT_FLOAT);
				it->rval->set_value<float>(std::stof(it->content));
				tokens.erase(it2);

				return true;
			}
		}
	}
	
	it->postfix.pop_front();
	return true;

}
expr::expression_token expr::EvaluateExpressionTokens(std::list<expression_token>& tokens)
{

	std::string result;
	std::list<expression_token>::iterator itr1, itr2 = tokens.begin();
	const auto& op_end = --tokens.end();
	OperatorPriority op, next_op;

	while (tokens.size() > 2) {
		itr1 = ++tokens.begin(); 
		itr2 = itr1;
		std::advance(itr2, 2);

		if (!itr1->op)
			throw std::exception("expected an expression");

		if (itr2 != tokens.end()) {
			do {
				op = GetOperatorPriority(itr1->content);
				next_op = GetOperatorPriority(itr2->content);

				if (next_op <= op || itr2 == op_end)
					break;
				
				std::advance(itr1, 2);
				std::advance(itr2, 2);

			} while (next_op > op);
		}
		itr2 = itr1;

		const auto& Operator = itr1->content;
		const auto& lval = (--itr1)->content;
		const auto& rval = (++itr2)->content;

		if (!ExpressionCompatibleOperands(*itr1, *itr2)) {
			throw std::exception(std::format("an operand of type \"{}\" is not compatible with \"{}\"", VarTypes[int(itr1->tokentype)], VarTypes[int(itr2->tokentype)]).c_str());
		}

		const auto& function = eval_funcs.find(Operator);
		
		if (function == eval_funcs.end())
			throw std::exception(std::format("unknown operator {}", Operator).c_str());

		//ExpressionMakeRvalue(*itr2);
		
		auto result = function->second(*itr1, *itr2);

		*itr2 = result;

		tokens.erase(itr1, itr2);
	}

	std::for_each(expression_postfixes.begin(), expression_postfixes.end(), ExpressionSetTempValue);
	expression_postfixes.clear();
	return *itr2;
}

void expr::ExpressionMakeRvalue(expression_token& token)
{
	
	if (token.is_lvalue()) {
		if (token.lval->ref->is_array())
			return;
		token.rval = std::shared_ptr<rvalue>(new rvalue(token.tokentype, (token.get_type() == VarType::VT_STRING ? strlen(token.get_string()) : 0u)));
		switch (token.tokentype) {
		case VarType::VT_INT:
			token.rval->set_value<int>(token.lval->ref->get_int());
			break;
		case VarType::VT_FLOAT:
			token.rval->set_value<float>(token.lval->ref->get_float());
			break;
		case VarType::VT_CHAR:
			token.rval->set_value<char>(token.lval->ref->get_char());
			break;
		case VarType::VT_STRING:
			//token.rval->set_value<char*>(token.lval->ref->get_string());
			//const std::string str = token.lval->ref->get_string();

			//token.rval->set_string((char*)str.substr(1, str.size() - 2).c_str());
			token.rval->set_string(token.lval->ref->get_string());
			break;
		}
		token.lval.reset();
		return;
	}

	if (token.get_type() == VarType::VT_STRING) {
		const std::string str = token.rval->get_string();
		std::cout << "rval str: " << str << '\n';
		token.rval->set_string((char*)str.substr(1, str.size() - 2).c_str());
		std::cout << "token rval str: " << token.get_string() << '\n';
	}

}
bool expr::ExpressionCompatibleOperands(const expression_token& left, const expression_token& right)
{
	const auto ltype = left.get_type();
	const auto rtype = right.get_type();

	if (ltype <= VarType::VT_VOID || rtype <= VarType::VT_VOID)
		return false;

	if (left.is_array() || right.is_array()) {

		if (ExpressionCompatibleArrayOperands(left, right))
			return true;

		const auto left_type = left.s_gettype();
		const auto right_type = right.s_gettype();

		throw std::exception(std::format("an operand of type \"{}\" is incompatible with \"{}\"", left_type, right_type).c_str());
		return false;

	}


	int leftFlag = 0;
	int rightFlag = 0;

	leftFlag |= int(ltype);
	rightFlag |= int(rtype);

	const int INT_FLAG = int(VarType::VT_INT);
	const int FLOAT_FLAG = int(VarType::VT_FLOAT);
	const int CHAR_FLAG = int(VarType::VT_CHAR);
	const int STRING_FLAG = int(VarType::VT_STRING);

	if (leftFlag & STRING_FLAG && rightFlag & STRING_FLAG) {
		return true;
	}
	else if (leftFlag & STRING_FLAG && (rightFlag & STRING_FLAG) == NULL || rightFlag & STRING_FLAG && (leftFlag & STRING_FLAG) == NULL)
		return false; 

	else if (leftFlag & FLOAT_FLAG && (rightFlag & FLOAT_FLAG || rightFlag & INT_FLAG)) {
		return true; 
	}
	else if (leftFlag & INT_FLAG && (rightFlag & INT_FLAG || rightFlag & FLOAT_FLAG || rightFlag & CHAR_FLAG)) {
		return true;
	}
	else if (leftFlag & CHAR_FLAG && (rightFlag & INT_FLAG)) {
		return true;
	}

	

	return false;

}
//this should be called if either operand IS an array
//function assumes that at least one operand is an array
bool expr::ExpressionCompatibleArrayOperands(const expression_token& left, const expression_token& right)
{
	const size_t ldepth = left.array_depth();
	const size_t rdepth = right.array_depth();

	if (ldepth != rdepth)
		return false;

	if (left.get_type() != right.get_type())
		return false;


	return true;

}
bool expr::ExpressionCompatiblePointerOperands(const expression_token& left, const expression_token& right)
{

}
//assumes that both operands are rvalues
void expr::ExpressionCastWeakerOperand(expression_token& left, expression_token& right)
{
	unsigned __int16 lengthA = left.is_lvalue() ? GetArrayDepth(left.lval->ref) : 0;
	unsigned __int16 lengthB = right.is_lvalue() ? GetArrayDepth(right.lval->ref) : 0;

	if (lengthA > 0 || lengthB > 0) { //array operands are only allowed for assignments
		throw std::exception("this is not a valid operation for the operands");
	}

	expression_token* stronger, *weaker;

	if (left.get_type() > right.get_type()) {
		stronger = &left;
		weaker = &right;
	}else if (left.get_type() < right.get_type()) {
		stronger = &right;
		weaker = &left;
	}
	else
		return;

	switch (stronger->get_type()) {
	case VarType::VT_CHAR:
		break;
	case VarType::VT_INT:
		break;
	case VarType::VT_FLOAT:
		const auto old_type = weaker->get_type();

		weaker->set_type(stronger->get_type());

		switch (old_type) {
		case VarType::VT_INT:
			std::cout << "an int gets promoted to a float\n";

			weaker->set_value<float>(weaker->implicit_cast<float>());
			weaker->set_type(VarType::VT_FLOAT);
			break;
		default:
			break;
		}
		break;

	}
	 
}

void expr::ExpressionImplicitCast(expression_token& left, expression_token& right)
{
	if (left.get_type() == right.get_type())
		return;

	if (!left.is_lvalue()) {
		return ExpressionCastWeakerOperand(left, right);
	}

	if (right.is_lvalue()) {
		throw std::exception("implicit cast called with right operand as an lvalue");
	}

	const auto left_v = left.lval->ref;

	if (left.size_of() < right.size_of()) {
		throw std::exception("cannot implicitly convert to a bigger type (narrowing conversion)");
		return;
	}
	
	//all other types will error before they reach this part of the code
	//right operand will be cast no matter what

	switch (left.get_type()) {
	case VarType::VT_INT:
		std::cout << "implicitly casting to 'int'\n";
		right.set_value<int>(right.implicit_cast<int>());
		right.set_type(VarType::VT_INT);
		break;
	case VarType::VT_FLOAT:
		std::cout << "implicitly casting to 'float'\n";
		right.set_value<float>(right.implicit_cast<float>());
		right.set_type(VarType::VT_FLOAT);
		break;	
	case VarType::VT_CHAR:
		break;
	}
}
void expr::ExpressionSetTempValue(temp_value_s& token) {

	if (!token.ref)
		return;

	switch (token.ref->get_type()) {
	case VarType::VT_INT:
		
		std::cout << token.ref->name << " gets the value " << token.ref->get_int() + (token.increment ? 1 : -1) << '\n';
		token.ref->set_value<int>(token.ref->get_int() + (token.increment ? 1 : -1));
		break;
	case VarType::VT_FLOAT:
		token.ref->set_value<float>(token.ref->get_float() + (token.increment ? 1 : -1));
		break;
	}

	token.ref = 0;

}