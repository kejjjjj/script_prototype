#include "statement.hpp"

statementType_e statement_resolve_datatype_ambiguity(code_segment_t tokens)
{
	if (tokens.it == tokens.end) 
		return statementType_e::DECLARATION;

	//a function style cast if true, but this doesn't even exist yet so :|
	//if ((++(tokens.it))->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_OPEN)
	//	return statementType_e::EXPRESSION;
		
	return statementType_e::DECLARATION;

}

statementType_e statement_determine(const code_segment_t& tokens)
{
	if (tokens.it->tt == tokenType::BUILT_IN_TYPE)
		return statement_resolve_datatype_ambiguity(tokens);

	else if (tokens.it->tt == tokenType::STATEMENT)
		return statementType_e::STATEMENT_KEYWORD;

	else if (tokens.it->tt == tokenType::FUNCTION)
		return statementType_e::FUNCTION_DECLARATION;

	//else if (tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN)
	//	return statementType_e::SCOPE;
	//else if (tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_CURLYBRACKET_CLOSE)
	//	return statementType_e::SCOPE_EXIT;


	return statementType_e::EXPRESSION;

}