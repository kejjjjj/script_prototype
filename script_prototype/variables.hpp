#pragma once

#ifndef scriptvars
#define scriptvars

#include "pch.h"

static const char* VarTypec_str3[3]
{
	"integer",
	"float",
	"string"
};

enum class VarType
{
	VT_INT,
	VT_FLOAT,
	VT_STRING,
	VT_INVALID
};

struct VariableValue
{
	int32_t integer;
	float value;
	std::string string;
};

class Variable
{
public:
	Variable(const std::string_view& _name, VarType _type, VariableValue init, int32_t _stacklevel);
	Variable() = default;
	~Variable() = default;

	void Initialize(const std::string_view& _name, VarType _type, VariableValue init, int32_t _stacklevel);

private:

	std::string_view name;
	VarType type;
	VariableValue value;
	int32_t stacklevel;


};



#endif