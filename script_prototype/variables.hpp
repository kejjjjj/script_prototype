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

inline std::vector<const char*> VarTypes = {"NO_TYPE", "int","float","string"},
								VarQualifiers = { "NO_QUALIFIER", "const" };



enum class VarType : char
{
	VT_INVALID,
	VT_INT,
	VT_FLOAT,
	VT_STRING,
	
};

struct VariableValue
{
	void* buffer;
	unsigned int buf_size;
	VarType datatype;
};


class Variable
{
public:
	Variable(const std::string_view& _name, VarType _type);
	Variable() = delete;
	~Variable() = default;

	std::string name;
	VarType type = VarType::VT_INVALID;
	std::vector<VariableValue> values;


};
struct ScriptBlock : Variable
{
	unsigned __int16 depth;
	//unsigned __int16 localVarsCount;
	std::vector<Variable> localVars;
};

struct hasher
{
	size_t operator()(std::pair<std::string_view, Variable> x) const
	{
		return ([&x]() {
			unsigned int hash = 5381;
			for (char c : x.first) {
				hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
			}
			return hash;
			})();
	}
};

inline std::unordered_map<std::string_view, Variable, hasher> stack_variables;

bool IsDataType(const std::string_view& str);
size_t GetDataType(const std::string_view& str);
size_t GetTypeQualifier(const std::string_view& str);

bool VariableInStack(const std::string_view& var);

Variable* FindVariableFromStack(const std::string_view& var);


bool CompatibleDataTypes(const VarType a, const VarType b);

bool IsConst(const std::string_view& v);

void DeclareVariable(const std::string_view& type, const std::string_view& name);
#endif