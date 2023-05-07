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

unsigned int GetVarTypeSize(const VarType t)
{
	switch (t) {
	case VarType::VT_VOID:
		return 0;
	case VarType::VT_INT:
	case VarType::VT_FLOAT:
		return 4;
	}

	return 1;
}

struct VariableValue
{
	std::shared_ptr<void*> buffer = 0;
	unsigned int buf_size = 0; 
};

class Array
{
public:
	Array(int initial_size, VarType t) {

		if (initial_size < 0 || initial_size >= INT_MAX)
			throw std::exception("invalid size for an array");

		if (initial_size == NULL)
			return;

		for (int i = 0; i < initial_size; i++) {
			value.push_back( {
				std::shared_ptr<void*>(new void*)
				, GetVarTypeSize(t)});
		}

	}

private:
	std::vector<VariableValue> value;
	VarType type;

};

class Variable : public Array
{
public:
	Variable(const std::string_view& _name, VarType _type, bool bArray = false);
	Variable() = delete;
	~Variable();

	const VariableValue& get_value() const
	{
		return value;
	}

	int get_int() const {
		return *reinterpret_cast<int*>(get_value().buffer.get());

	}
	float get_float() const {
		return *reinterpret_cast<float*>(get_value().buffer.get());
	}
	char* get_string() const {
		return reinterpret_cast<char*>(get_value().buffer.get());
	}

	void set_int(int value) {
		*reinterpret_cast<int*>(get_value().buffer.get()) = value;
	}
	void set_float(float value) {
		*reinterpret_cast<float*>(get_value().buffer.get()) = value;
	}
	void set_string(char* value) {
#pragma warning(suppress : 4996);
		strcpy(reinterpret_cast<char*>(get_value().buffer.get()), value);
	}
	template <typename T>
	void set_value(const T v) {
		*reinterpret_cast<T*>(get_value().buffer.get()) = v;
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