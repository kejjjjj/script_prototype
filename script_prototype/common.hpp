#pragma once

#ifndef ccommon
#define ccommon

#include "pch.h"

inline std::vector<std::string> compilerlog;
inline void CompilerError()
{

	std::string beg = std::format("Line: {}\nColumn: {}\n\nDescription:\n", fs::file.lines_read, fs::file.current_column);
	
	compilerlog.insert(compilerlog.begin(), beg);

	beg.clear();
	for (const auto& i : compilerlog) {
		beg += i;
	}

	MessageBoxA(NULL, beg.c_str(), "Compile Error!", MB_ICONERROR);
	compilerlog.clear();
	exit(-1);
	return;
}

template <typename T, typename ... Param>
inline void CompilerError(const T& t, const Param& ... param)
{
	std::stringstream ss;
	ss << t;
	compilerlog.push_back(ss.str());

	//std::cout << t << '\n';
	CompilerError(param...);

}

//the higher the value the higher the priority
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
//inline OperatorPriority operatorHash[513];

struct Substr_s
{
	size_t count_opening;
	size_t count_closing;
	size_t opening; //string index where the ( is
	size_t strlength; //number of characters after opening until )
	std::string result_string;
};

Substr_s GetStringWithinCharacters(const std::string_view& expr, const char s, const char e);

OperatorPriority GetOperatorPriority(const std::string_view& ops);
bool ValidNumber(const std::string_view& expr);
bool IsInteger(const std::string_view& expr);

bool IsAnOperator(const std::string_view& op);
bool SatisfiesOperator(const std::string_view& op);
bool IsAnOperator2(const std::string_view& op);
bool IsUnaryOperator(const std::string_view& op);
bool IsPostfixOperator(const std::string_view& op);
bool StringIsBlank(const std::string_view& s);
#endif