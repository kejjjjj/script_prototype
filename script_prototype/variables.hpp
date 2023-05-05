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

inline std::vector<const char*> VarTypes = {"NO_TYPE", "void", "int","float","string"},
								VarQualifiers = { "NO_QUALIFIER", "const" };



enum class VarType : char
{
	VT_INVALID,
	VT_VOID,
	VT_INT,
	VT_FLOAT,
	VT_STRING,
	
};

struct VariableValue
{
	void* buffer;
	unsigned int buf_size = 0; 
};


class Variable
{
public:
	Variable(const std::string_view& _name, VarType _type);
	Variable() = delete;
	~Variable();

	const VariableValue& get_value() const
	{
		return value;
	}

	int get_int() const {
		return *reinterpret_cast<int*>(get_value().buffer);

	}
	float get_float() const {
		return *reinterpret_cast<float*>(get_value().buffer);
	}
	double get_double() const {
		return *reinterpret_cast<double*>(get_value().buffer);
	}
	char* get_string() const {
		return reinterpret_cast<char*>(get_value().buffer);
	}

	void set_int(int value) {
		*reinterpret_cast<int*>(get_value().buffer) = value;
	}
	void set_float(float value) {
		*reinterpret_cast<float*>(get_value().buffer) = value;
	}
	void set_double(double value) {
		*reinterpret_cast<double*>(get_value().buffer) = value;
	}
	void set_string(char* value) {
#pragma warning(suppress : 4996);
		strcpy(reinterpret_cast<char*>(get_value().buffer), value);
	}
	template <typename T>
	void set_value(const T v) {
		*reinterpret_cast<T*>(get_value().buffer) = v;
	}
	void set_type(const VarType atype) {
		type = atype;
	}
	auto get_type() const { return type; }
	std::string name;
private:
	
	VarType type = VarType::VT_INVALID;
	VariableValue value;
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

inline std::unordered_map<std::string, Variable> stack_variables;

bool IsDataType(const std::string_view& str);
size_t GetDataType(const std::string_view& str);
size_t GetTypeQualifier(const std::string_view& str);

bool VariableInStack(const std::string_view& var);

Variable* FindVariableFromStack(const std::string_view& var);


bool CompatibleDataTypes(const VarType a, const VarType b);

bool IsConst(const std::string_view& v);

void DeclareVariable(const std::string_view& type, const std::string_view& name);
#endif