#pragma once

#ifndef __compvar
#define __compvar

#include "pch.h"

namespace cv
{
	bool ParseDeclarationType(const std::string_view& full_statement);
	bool ParseTypeQualifier(const std::string_view& full_statement);
	bool ParseIdentifier(const std::string_view& full_statement);
	bool ParseInitializer(const std::string_view& full_statement);
	std::string ParseOperator(std::string::iterator& it);
}

#endif