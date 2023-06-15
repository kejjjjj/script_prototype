#pragma once

#ifndef __datatypes
#define __datatypes

#include "pch.h"

inline std::vector<const char*> VarTypes = { "NO_TYPE", "void", "char", "int","float","string" },
VarQualifiers = { "NO_QUALIFIER", "const" };


enum class VarType : char
{
	VT_INVALID,
	VT_VOID,
	VT_CHAR,
	VT_INT,
	VT_FLOAT,
	VT_STRING

};

inline std::unordered_map<std::string, VarType> literal_suffix =
{
	{ "c", VarType::VT_CHAR }
};


#endif