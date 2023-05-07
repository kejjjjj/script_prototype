#include "pch.h"

std::string expr::EvaluateEntireExpression(const std::string& str)
{
	Parenthesis_s par = GetStringWithinParentheses(str);

	if (par.result_string == "empty") {
		par.result_string = "";
	}

	//parentheses exists
	if (!par.result_string.empty()) {
		std::string copy = str;

		auto result = EvaluateExpression(par.result_string);

		copy.erase((size_t)par.opening, (size_t)par.strlength + 2ull);
		copy.insert((size_t)par.opening, ' ' + result + ' '); //spacebar fixes cases like (500+200)0 (without the space this would turn into 7000)
		

		result = EvaluateEntireExpression(copy);
		return result;
	}
	auto result = expr::EvaluateExpression(str);
	return result;
}
std::string expr::EvaluateExpression(const std::string& str)
{
	rules.next_postfix = false;
	rules.next_unary = true;
	rules.next_operator = false;
	srules.expecting_identifier = false;

	syntax.ClearFlag(S_END_OF_NUMBER);
	if (ValidNumber(str)) {
		return std::string(str);
	}
	
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

	//std::cout << "made this token: " << '\n';

	//for (auto& i : tokens) {

	//	std::cout << '(';
	//	if (!i.op) {
	//		for (auto& prefix : i.prefix) {
	//			std::cout << prefix;
	//		}
	//		std::cout << i.content;
	//		for (auto& postfix : i.postfix) {
	//			std::cout << postfix;
	//		}
	//	}
	//	else
	//		std::cout << i.content;

	//	std::cout << ")\n";
	//}
	
	return EvaluateExpressionTokens(tokens);

}
void expr::TokenizeExpression(std::string::iterator& it, std::string::iterator& end, std::list<expression_token>& tokens)
{
	expression_token expr_token;


	bool kill_loop = false;
	rules.next_unary = true;
	rules.next_postfix = false;
	bool break_on_whitespace = false;
	rules.ignore_postfix = false;
	rules.operator_allowed = true;
	if (!tokens.empty()) {
		if (tokens.back().postfix.front() == ".") { //for cases like 2. + 1;
			rules.next_unary = false;
			//rules.next_operator = true;
		}
	}

	while (!kill_loop && it != end) {

		const token_t token = cec::Compiler_ReadToken(it, '\0', end);
		//std::cout << "token: " << token.value << '\n';
		
		switch (token.t_type) {
		case token_t::tokentype::STRING:
		case token_t::tokentype::DIGIT:

			if (expr_token.tokentype == VarType::VT_INVALID) {
				if(token.t_type == token_t::tokentype::STRING)
					expr_token.tokentype = VarType::VT_STRING;
				else
					expr_token.tokentype = VarType::VT_INT;
			}

			rules.operator_allowed = true;

			if (rules.next_operator) {
				throw std::exception("expected an expression");
			}
			if (!rules.next_unary && rules.next_postfix) {
				it -= token.value.length();
				kill_loop = true;
				break;
			}
			rules.next_unary = false;
			rules.next_postfix = true;
			expr_token.content = token.value;
			//break_on_whitespace = true;
			if (rules.ignore_postfix) {
				kill_loop = true;
				break;
			}

			break;
		case token_t::tokentype::OPERATOR:

			if (expr_token.tokentype == VarType::VT_INVALID)
				expr_token.tokentype = VarType::VT_INVALID;

			if (!rules.operator_allowed) {
				throw std::exception("expected an integral type instead of operator");
			}

			rules.ignore_postfix = false;
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

				if (token.value == ".") {
					rules.ignore_postfix = true;
					rules.operator_allowed = false;
				}

			}
			else if (IsPostfixOperator(token.value) && rules.next_postfix) {

				expr_token.postfix.push_back(token.value);
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
			if (expr_token.content.empty())
				expr_token.whitespace = true;
			else if(break_on_whitespace) {
				--it;
				kill_loop = true;
			}
			break;
		}
	}
	rules.next_operator = !rules.next_operator;
	if (!expr_token.content.empty()) {
		expr_token.whitespace = false;
		tokens.push_back(expr_token);
	}
	if(it != end)
		TokenizeExpression(it, end, tokens);

	

	return;
}
void expr::SetTokenValueCategory(expression_token& token)
{


	if (token.op)
		return;



	if (token.tokentype != VarType::VT_STRING) {
		
		token.rval = std::shared_ptr<rvalue>(new rvalue(token.tokentype));
		token.rval->set_value<int>(std::stoi(token.content));
		return;
	}

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

	std::string op;
	op.push_back(token.postfix.front().front());
	token.content = Eval(token.content, "1", op);
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

	if (UnaryArithmeticOp(token.prefix.back())) {
		if (token.is_rvalue())
			throw std::exception("increment/decrement operand must be an lvalue");

		EvaluatePrefixArithmetic(token, token.prefix.back() == "++");
		token.prefix.pop_back();
		return EvaluatePrefix(it, end);
	}

	expression_token r_operand;
	r_operand.rval = std::shared_ptr<rvalue>(new rvalue(VarType::VT_INT));
	
	ExpressionMakeRvalue(token);

	switch (token.prefix.back().front())
	{
		case '-':
			r_operand.rval->set_value<int>(-1);
			token.content = eval_funcs.find("*")->second(token, r_operand).content;
			break;
		case '+':
			r_operand.rval->set_value<int>(1);
			token.content = eval_funcs.find("*")->second(token, r_operand).content;
			break;
		case '~':
			if (!token.is_integral())
				throw std::exception("bitwise complement expects an integral operand");

			token.set_value(~token.get_int());
			token.content = std::to_string(token.get_int());

			break;
		case '!':
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
	
	auto& value = token.lval->ref->get_value();

	if (type == VarType::VT_FLOAT) {


		*reinterpret_cast<float*>(*value.buffer) += increment == true ? 1 : -1;
		token.content = std::to_string(*reinterpret_cast<float*>(*value.buffer));

	}
	else {
		*reinterpret_cast<int*>(*value.buffer) += increment == true ? 1 : -1;
		token.content = std::to_string(*reinterpret_cast<int*>(*value.buffer));
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
std::string expr::EvaluateExpressionTokens(std::list<expression_token>& tokens)
{
	if (tokens.size() == 1)
		return tokens.front().content;

	std::string result;
	std::list<expression_token>::iterator itr1, itr2;
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
				op = GetOperandPriority(itr1->content);
				next_op = GetOperandPriority(itr2->content);

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

		if (!ExpressionCompatibleOperands(itr1->tokentype, itr2->tokentype)) {
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
	return itr2->content;
}

void expr::ExpressionMakeRvalue(expression_token& token)
{
	if (token.is_lvalue()) {
		token.rval = std::shared_ptr<rvalue>(new rvalue(token.tokentype));
		switch (token.tokentype) {
		case VarType::VT_INT:
			token.rval->set_value<int>(token.lval->ref->get_int());
			break;
		case VarType::VT_FLOAT:
			token.rval->set_value<float>(token.lval->ref->get_float());
			break;
		case VarType::VT_STRING:
			token.rval->set_value<char*>(token.lval->ref->get_string());
			break;
		}
		token.lval.reset();
		//std::cout << "deleting lvalue\n";
		return;
	}



}
bool expr::ExpressionCompatibleOperands(const VarType left, const VarType right)
{
	if (left <= VarType::VT_VOID || right <= VarType::VT_VOID)
		return false;

	int leftFlag = 0;
	int rightFlag = 0;

	leftFlag |= int(left);
	rightFlag |= int(right);

	const int INT_FLAG = int(VarType::VT_INT);
	const int FLOAT_FLAG = int(VarType::VT_FLOAT);
	const int STRING_FLAG = int(VarType::VT_STRING);

	if (leftFlag & STRING_FLAG && rightFlag & STRING_FLAG) {
		return true;
	}
	else if (leftFlag & FLOAT_FLAG && (rightFlag & FLOAT_FLAG || rightFlag & INT_FLAG)) {
		return true;
	}
	else if (leftFlag & INT_FLAG && (rightFlag & INT_FLAG || rightFlag & FLOAT_FLAG)) {
		return true;
	}

	return false;

}
void expr::ExpressionCastWeakerOperand(expression_token& left, expression_token& right)
{
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

	case VarType::VT_INT:
		break;
	case VarType::VT_FLOAT:
		auto old_type = weaker->get_type();

		weaker->set_type(stronger->get_type());

		switch (old_type) {
		case VarType::VT_INT:
			std::cout << "an int gets promoted to a float\n";
			weaker->set_value<float>(weaker->get_int());
			break;
		default:
			break;
		}
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
		token.ref->set_value<float>(token.ref->get_int() + (token.increment ? 1 : -1));
		break;
	}

	token.ref = 0;

}