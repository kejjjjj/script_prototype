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

//void CompilerError(std::string str, ...);

enum OperatorPriority
{
	LOW,		//	addition/subtraction
	MEDIUM,		//	multiplication/division
	HIGH,		//  exponents
	HIGHEST,	//  parenthesis

};
struct Parenthesis_s
{
	int32_t count_opening;
	int32_t count_closing;
	int32_t opening; //string index where the ( is
	int32_t strlength; //number of characters after opening until )
	std::string_view result_string;
};
std::string RemoveFromString(std::string& str, char a);
SIZE_T TokenizeString(std::string& expr, char delim, std::vector<std::string>& tokens);
SIZE_T GetStringTokens(std::string& expr, char delim);
SIZE_T GetStringOperandTokens(std::string_view& expr);
SIZE_T TokenizeStringOperands(const std::string_view& expr, std::vector<std::string>& tokens);
Parenthesis_s GetStringWithinParenthesis(const std::string_view& expr);
float Eval(const float& a, const float& b, char op);
OperatorPriority GetOperandPriority(char op);
bool ValidNumber(const std::string_view& expr);

std::string RemoveBlank(const std::string_view& expr);

size_t RemoveBlank(std::string_view& expr, std::string& out);
std::string RemoveBlanksFromBeginningAndEnd(std::string& in);
#endif