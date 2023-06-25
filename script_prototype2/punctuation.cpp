#include "punctuation.hpp"

bool is_unary_operator(const punctuation_e p)
{
	if (p == punctuation_e::P_UNKNOWN)
		return false;

	if (p >= P_INCR && p <= P_DECR)
		return true;

	if (p < punctuation_e::P_PAR_OPEN)
		return true;

	return false;
}

bool is_postfix_operator(const punctuation_e p)
{
	if (p == punctuation_e::P_UNKNOWN)
		return false;

	if (p >= P_INCR && p <= P_DECR)
		return true;

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