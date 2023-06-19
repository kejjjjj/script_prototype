#pragma once

#ifndef _punct
#define _punct

#include "script.hpp"


enum OperatorPriority
{
	FAILURE,
	ASSIGNMENT,		//	= += -= *= /= %= >>= <<= &= ^= |=
	CONDITIONAL,	//	?:
	LOGICAL_OR,		//	||
	LOGICAL_AND,	//	&&
	BITWISE_OR,		//  | 
	BITWISE_XOR,	//	^
	BITWISE_AND,	//  & 
	EQUALITY,		//	< <= > >=
	RELATIONAL,		//	== !=
	SHIFT,			//  <<>>
	ADDITIVE,		//	+-
	MULTIPLICATIVE,	//	* / %
	UNARY,			//  + - ! ~ ++ - - (type)* & sizeof	
	POSTFIX,		//  () [] -> . ++ - -	

};
enum punctuation_e : size_t
{
	P_UNKNOWN,
	P_ADD,
	P_SUB,
	P_SEMICOLON
};

struct punctuation_t
{
	std::string identifier;
	punctuation_e punc = P_UNKNOWN;
};

constexpr punctuation_t punctuations[] =
{
	{"+", P_ADD},
	{"-", P_SUB},

	{";", P_SEMICOLON }
};

bool is_unary_operator(const punctuation_e p);
bool is_postfix_operator(const punctuation_e p);
bool satisfies_operator(const punctuation_e p);

#endif