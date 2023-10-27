#include "expression.hpp"
#include "o_postfix.hpp"
#include "o_standard.hpp"
#include "parsing_utils.hpp"

std::list<expression_token> tokenize_expression(code_segment_t& code, scr_scope_t* scope);
bool parse_expression(scr_scope_t* scope, code_segment_t& tokens, std::list<expression_token>& sortedTokens);
bool parse_operator(code_segment_t& tokens, std::list<expression_token>& sortedTokens);
bool expression_parse_parentheses(scr_scope_t* scope, expression_token& token, code_segment_t& tokens);

expression_token evaluate_expression_tokens(std::list<expression_token>& sortedTokens);

postfix_squarebracket postfix_square_bracket(code_segment_t& tokens);
postfix_parenthesis postfix_opening_parenthesis(code_segment_t& tokens);

expression_token eval_expression(scr_scope_t* scope, code_segment_t tokens)
{

	LOG("EvaluateEntireExpression(");
	for (auto it = tokens.it; it != tokens.end; ++it) {
		LOG(it->string << ' ');
	}
	LOG(")\n");

	++tokens.end;

	auto sortedtokens = tokenize_expression(tokens, scope);

	std::for_each(sortedtokens.begin(), sortedtokens.end(), [scope](expression_token& t) {
		t.eval_postfix(scope);
		t.eval_prefix();
	});

	return evaluate_expression_tokens(sortedtokens);

}


std::list<expression_token> tokenize_expression(code_segment_t& tokens, scr_scope_t* scope)
{
	std::list<expression_token> sortedTokens;

	if (tokens.it == tokens.end)
		return sortedTokens;

	while (tokens.it != tokens.end) {
		if (!parse_expression(scope, tokens, sortedTokens)) {
			throw scriptError_t(&*tokens.it, std::format("expected an expression instead of \"{}\"\n", tokens.it->string));
		}

		if (tokens.it == tokens.end)
			break;

		if (!parse_operator(tokens, sortedTokens)) {
			if (tokens.it == tokens.end)
				throw scriptError_t(&*tokens.it, std::format("expected an expression before \"{}\"\n", tokens.it->string));

			throw scriptError_t(&*tokens.it, std::format("expected an operator or \";\" instead of \"{}\"\n", tokens.it->string));

		}

	}
	if (sortedTokens.back().op) {
		throw scriptError_t(&*tokens.it, std::format("unexpected end-of-expression \"{}\"", tokens.it->string));
	}

	return sortedTokens;

}
bool parse_expression(scr_scope_t* scope, code_segment_t& tokens, std::list<expression_token>& sortedTokens)
{
	auto& it = tokens.it;
	expression_token token;
	const auto token_peek_unary = [&]()
	{
		if (it == tokens.end || it->tt != tokenType::PUNCTUATION)
			return false;

		const auto punctuation = static_cast<punctuation_e>(LOWORD(it->extrainfo));

		if (bad_unary_operator(punctuation)) {
			throw scriptError_t(&*it, std::format("expected unary operator instead of \"{}\"", it->string));
		}

		if (!is_unary_operator(punctuation)){
			return false;
		}

		

		token.insert_prefix(*it);
		it++;

		return true;
	};
	const auto token_peek_name = [&]()
	{
		if (it == tokens.end)
			return false;

		if (it->tt == tokenType::PUNCTUATION )
			return false;

		token.set_token(*it);
		it++;

		return true;
	};
	const auto token_peek_postfix = [&]()
	{
		if (it == tokens.end || it->tt != tokenType::PUNCTUATION)
			return false;

		if (!is_postfix_operator(static_cast<punctuation_e>(LOWORD(it->extrainfo)))) {
			return false;
		}

		if (tokens.it->tt == tokenType::PUNCTUATION) {

			switch (LOWORD(tokens.it->extrainfo)) {
			case P_BRACKET_OPEN:
				token.insert_postfix(std::move(std::shared_ptr<postfix_squarebracket>(new postfix_squarebracket(postfix_square_bracket(tokens)))));
				return true;
			case P_PAR_OPEN:
				token.insert_postfix(std::move(std::shared_ptr<postfix_parenthesis>(new postfix_parenthesis(postfix_opening_parenthesis(tokens)))));
				return true;
			default:
				break;
			}

		}
		
		
		it++;


		return true;
	};

	while (token_peek_unary()) {}

	if (!token_peek_name()) {

		if (expression_parse_parentheses(scope, token, tokens) == false) {

			if (token.prefix.empty() == false)
				throw scriptError_t(&*it, "expected an expression because parentheses failed lol");

			return false;
		}

		//else if (token.prefix.empty() == false)
		//	throw scriptError_t(&*it, "expected an expression because there is a prefix lol");

	}

	while (token_peek_postfix()) {}

	token.set_scope(scope);
	token.set_value_category();

	sortedTokens.push_back(token);

	return true; //expression has valid syntax
}
bool parse_operator(code_segment_t& tokens, std::list<expression_token>& sortedTokens)
{
	

	auto& it = tokens.it;
	expression_token token;

	if (it == tokens.end)
		return false;

	if (it->tt != tokenType::PUNCTUATION) {
		return false;
	}

	if (!satisfies_operator(static_cast<punctuation_e>(LOWORD(it->extrainfo)))) {
		return false;
	}
	token.set_token(*it);
	token.op = LOWORD(it->extrainfo);
	token.op_priority = HIWORD(it->extrainfo);
	++it;

	//if (it == tokens.end) //last token should NOT be an operator
	//	return false;
	token.set_value_category();
	sortedTokens.push_back(token);

	return true; //operator has valid syntax
}
bool expression_parse_parentheses(scr_scope_t* scope, expression_token& token, code_segment_t& tokens)
{
	if ((tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_OPEN) == false)
		return false;

	auto substr = find_matching_parenthesis(tokens);

	if (!substr) {
		throw scriptError_t(&*tokens.it, "empty expression is not allowed here");
	}

	std::list<token_t*> backup_prefix = token.prefix;
	decltype(token.postfix) backup_postfix = token.postfix;

	token = eval_expression(scope, substr.value());

	//token = expression_t(block, statement).EvaluateEntireExpression();

	token.prefix = backup_prefix;
	token.postfix = backup_postfix;


	tokens.it = substr.value().end;
	++tokens.it; //)
	++tokens.it; //the token after )

	LOG("continuing iteration from " << tokens.it->string << '\n');
	return true;
}
expression_token evaluate_expression_tokens(std::list<expression_token>& sortedTokens)
{
	std::list<expression_token>::iterator itr1, itr2 = sortedTokens.begin();
	const auto& op_end = --sortedTokens.end();
	OperatorPriority op{}, next_op{};



	while (sortedTokens.size() > 2) {
		itr1 = ++sortedTokens.begin();
		itr2 = itr1;
		std::advance(itr2, 2);

		if (!itr1->op)
			throw scriptError_t(&itr1->get_token(), "expected an expression");

		if (itr2 != sortedTokens.end()) {
			do {
	/*			if (!itr2->op || !itr1->op) {
					throw scriptError_t(&itr2->get_token(), "how the FUCK are you not an operator");
				}*/
				op = static_cast<OperatorPriority>(itr1->op_priority);
				next_op = static_cast<OperatorPriority>(itr2->op_priority);

				if (itr2 == sortedTokens.end() || itr2 == op_end || next_op <= op)
					break;

				std::advance(itr1, 2);
				std::advance(itr2, 2);

			} while (next_op > op);

		}
		itr2 = itr1;


		auto& Operator	= itr1->get_token();
		auto& leftVal	= --itr1;
		auto& rightVal	= ++itr2;

		using FunctionType = std::function<expression_token(expression_token&, expression_token&)>;
		using OptionalFunctionType = std::optional<FunctionType>;

		const auto& evalFunctions = evaluationFunctions::getInstance();
		const auto punctuationType = static_cast<punctuation_e>(LOWORD(Operator.extrainfo));
		const OptionalFunctionType function = evalFunctions.find_function(punctuationType);

		if (!function.has_value())
			throw scriptError_t(&Operator, std::format("no evaluation function for the \"{}\" operator", Operator.string));

		*itr2 = function.value()(*leftVal, *rightVal);
	

		sortedTokens.erase(itr1, itr2);
	}

	return std::move(*itr2);
}

postfix_squarebracket postfix_square_bracket(code_segment_t& tokens)
{
	auto substr = find_matching_square_bracket(tokens);

	if (substr) {
		tokens.it = substr.value().end;
		tokens.it++; //go to ]
		tokens.it++; //skip the ]

		postfix_squarebracket v;
		v.expression = substr.value();
		return v;
	}
	throw scriptError_t(&*tokens.it, "expected an expression after the [ operator");
	
}
postfix_parenthesis postfix_opening_parenthesis(code_segment_t& tokens)
{
	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
		};

	auto substr = find_matching_parenthesis(tokens);
	postfix_parenthesis v;

	bool comma_used = false;

	auto next_it = tokens.it;

	if (substr) { //if the function call has > 0 arguments
		code_segment_t copy = substr.value();

		tokens.it = substr.value().end;
		tokens.it++; //go to )
		copy.end = tokens.it;
		tokens.it++; //skip the )
		code_segment_t arg;

		do {
			comma_used = false;
			auto arg_opt = find_appropriate_comma_delimiter(copy);

			if (!arg_opt) { //no expression
				v.args.push_back(std::nullopt);
				++copy.it;
				continue;
			}

			arg = arg_opt.value();
			
			if (is_comma(*arg.end)) //don't include the comma in the expression, so remove it
				arg.end--;

			v.args.push_back(arg);
			v.args.back()->print();

			copy.it = arg_opt.value().end; //go back to the parsed end

			if (is_comma(*copy.it))
				comma_used = true;

			copy.it++; //skip one character to avoid an infinite loop

		} while (copy.it != copy.end);

		if(comma_used) // a lazy way to check if the last argument was valid
			v.args.push_back(std::nullopt);

		LOG("calling a function with " << v.args.size() << " args!\n");

		return v;
	}

	LOG("NO ARGS\n");

	tokens.it++; //skip the (
	tokens.it++; //skip the )

	return v;

	
}