#include "punctuation.hpp"

bool is_unary_operator(const punctuation_e p)
{
	if (p == punctuation_e::P_UNKNOWN)
		return false;

	if (p < punctuation_e::P_SEMICOLON)
		return true;

	return false;
}

bool is_postfix_operator(const punctuation_e p)
{
	if (p == punctuation_e::P_UNKNOWN)
		return false;



	return false;
}

bool satisfies_operator(const punctuation_e p)
{
	if (p == punctuation_e::P_UNKNOWN)
		return false;

	if (p < punctuation_e::P_SEMICOLON)
		return true;

	return false;
}