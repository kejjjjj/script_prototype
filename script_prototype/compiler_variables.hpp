#pragma once

#ifndef __compvar
#define __compvar

#include "pch.h"

namespace cv
{
	bool ParseVariableDeclaration(const std::string_view& full_statement);

}

#endif