#include "statement.hpp"

statementType_e statement_resolve_datatype_ambiguity(token_statement_t tokens)
{
	if (tokens.it == tokens.end) 
		return statementType_e::DECLARATION;

	//a function style cast if true, but this doesn't even exist yet so :|
	//if ((++(tokens.it))->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_OPEN)
	//	return statementType_e::EXPRESSION;
		
	return statementType_e::DECLARATION;

}

statementType_e statement_determine(const token_statement_t& tokens)
{
	if (tokens.it->tt == tokenType::BUILT_IN_TYPE)
		return statement_resolve_datatype_ambiguity(tokens);

	else if (tokens.it->tt == tokenType::STATEMENT)
		return statementType_e::STATEMENT_KEYWORD;

	//else if (tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN)
	//	return statementType_e::SCOPE;
	//else if (tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_CURLYBRACKET_CLOSE)
	//	return statementType_e::SCOPE_EXIT;


	return statementType_e::EXPRESSION;

}