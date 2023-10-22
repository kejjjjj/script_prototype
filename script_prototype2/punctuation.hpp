#pragma once

#ifndef _punct
#define _punct

#include "script.hpp"


enum OperatorPriority : WORD
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
enum punctuation_e : WORD
{
	P_UNKNOWN,
	P_ADD,
	P_SUB,
	
	P_INCR,
	P_DECR,

	P_ASSIGN,

	P_PAR_OPEN,
	P_PAR_CLOSE,

	P_BRACKET_OPEN,
	P_BRACKET_CLOSE,

	P_CURLYBRACKET_OPEN,
	P_CURLYBRACKET_CLOSE,

	P_ARROW,

	P_COMMA,
	P_SEMICOLON
};

struct punctuation_t
{
	std::string identifier;
	punctuation_e punc = P_UNKNOWN; //LOWORD
	OperatorPriority priority = FAILURE; //HIWORD
};

constexpr punctuation_t punctuations[] =
{
	//needs to be sorted from longest string to shortest

	{"++", P_INCR, FAILURE},
	{"--", P_DECR, FAILURE},

	{"->", P_ARROW, FAILURE},

	{"+", P_ADD, ADDITIVE},
	{"-", P_SUB, ADDITIVE},

	{"=", P_ASSIGN, ASSIGNMENT},

	{"(", P_PAR_OPEN, FAILURE},
	{")", P_PAR_CLOSE, FAILURE},

	{"[", P_BRACKET_OPEN, FAILURE},
	{"]", P_BRACKET_CLOSE, FAILURE},

	{"{", P_CURLYBRACKET_OPEN, FAILURE},
	{"}", P_CURLYBRACKET_CLOSE, FAILURE},

	{",", P_COMMA},
	{";", P_SEMICOLON }
};

bool is_unary_operator(const punctuation_e p);
bool bad_unary_operator(const punctuation_e p);

bool is_postfix_operator(const punctuation_e p);
bool satisfies_operator(const punctuation_e p);

#endif