#pragma once

#ifndef __cws
#define __cws

#include "pch.h"

struct whenstatement_s
{
	bool else_is_allowed = false;

};

namespace cws
{

	bool ParseWhenStatement(const std::string_view& expr);
	bool EvaluateExpression(const std::string_view& expr);
	bool ParseExpression(std::string& expr);
	bool ParseExpressionNumbers(std::string& expr);
	std::string CleanupExpression(const std::string_view& str);
	void TestExpression(const std::string_view& str);

	whenstatement_s statement;

}

#endif