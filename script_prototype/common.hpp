#pragma once

#ifndef ccommon
#define ccommon

#include "pch.h"

void CompilerError(std::string str, ...);

std::string RemoveFromString(std::string& str, char a);
SIZE_T TokenizeString(std::string& expr, char delim, std::vector<std::string>& tokens);
SIZE_T GetStringTokens(std::string& expr, char delim);
std::string RemoveBlank(std::string& expr);
size_t RemoveBlank(std::string& expr, std::string& out);
std::string RemoveBlanksFromBeginningAndEnd(std::string& in);
#endif