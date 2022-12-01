#pragma once

#ifndef ccommon
#define ccommon

#include "pch.h"

void CompilerError(std::string str, ...);

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
	int32_t closing; //string index where the ) is
	std::string result_string;
};
std::string RemoveFromString(std::string& str, char a);
SIZE_T TokenizeString(std::string& expr, char delim, std::vector<std::string>& tokens);
SIZE_T GetStringTokens(std::string& expr, char delim);
SIZE_T GetStringOperandTokens(std::string& expr);
SIZE_T TokenizeStringOperands(std::string& expr, std::vector<std::string>& tokens);
Parenthesis_s GetStringWithinParenthesis(std::string& expr);
float Eval(float a, float b, char op);
OperatorPriority GetOperandPriority(char op);

std::string RemoveBlank(std::string& expr);
size_t RemoveBlank(std::string& expr, std::string& out);
std::string RemoveBlanksFromBeginningAndEnd(std::string& in);
#endif