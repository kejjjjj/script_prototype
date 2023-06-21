#include "statement.hpp"

statementType_e statement_resolve_datatype_ambiguity(token_statement_t tokens)
{
	if (tokens.it == tokens.end) 
		return statementType_e::DECLARATION;

	if ((++(tokens.it))->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_OPEN)
		return statementType_e::EXPRESSION;
		
	return statementType_e::DECLARATION;

}

statementType_e statement_determine(const token_statement_t& tokens)
{
	if (tokens.it->tt == tokenType::BUILT_IN_TYPE)
		return statement_resolve_datatype_ambiguity(tokens);


}