#pragma once

#ifndef scriptvars
#define scriptvars

#include "pch.h"

//static const char* VarTypec_str[4]
//{
//	"integer",
//	"float",
//	"string",
//	"unknown"
//};

inline std::vector<const char*> VarTypes = {"NO_TYPE", "int","float","string"};

enum class VarType
{
	VT_INVALID,
	VT_INT,
	VT_FLOAT,
	VT_STRING,
	
};

struct VariableValue
{
	int32_t integer = 0;
	float value = 0.f;
	std::string string;
};

class Variable
{
public:
	Variable(const std::string_view& _name, VarType _type, VariableValue init, int32_t _stacklevel);
	Variable() = default;
	~Variable() = default;

	void Initialize(const std::string_view& _name, VarType _type, VariableValue init, int32_t _stacklevel);

//private:

	std::string name;
	VarType type = VarType::VT_INVALID;
	VariableValue value;
	int32_t stacklevel = 0;


};

std::string GetVariableTypeString(const std::string expr);
bool IsDataType(const std::string_view& str);
size_t GetDataType(const std::string_view& str);
bool VariableInStack(const std::string_view& var);

Variable* FindVariableFromStack(const std::string_view& var);

//assumes the whole string before the operator
bool IsVariableInitialization(const std::string_view& expr);
bool VariableNameIsLegal(const std::string_view& var);

#endif