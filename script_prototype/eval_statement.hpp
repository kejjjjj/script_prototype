#pragma once

#ifndef evl_stmnt
#define evl_stmnt

#include "pch.h"

enum class StatementType
{
	NO_STATEMENT,
	VARIABLE_EXPRESSION,
	WHEN_STATEMENT
};

inline static const std::vector<std::pair<std::string, StatementType>> statement_type = { std::make_pair("when", StatementType::WHEN_STATEMENT) };

StatementType Compile_EvaluateStatement(const std::string_view& expr);
StatementType Compile_EvaluateStatement2(const std::string_view& expr);

std::string FillStatement(char start, std::fstream& f);

void ParseStatement(const StatementType stype, const std::string_view& statement);


#endif