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

inline unsigned int GetVarTypeSize(const VarType t)
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


struct array_declr_data
{
	int numElements = 0;
	VarType type = VarType::VT_INVALID;
};

class Array
{
public:
	Array(array_declr_data* initializer_data) {

		if (!initializer_data)
			return;

		if (initializer_data->type == VarType::VT_INVALID)
			return;

		if (initializer_data->numElements < 1 || initializer_data->numElements >= INT_MAX)
			throw std::exception("invalid size for an array");

		type = initializer_data->type;

		for (int i = 0; i < initializer_data->numElements; i++) {
			value.push_back({ std::make_shared<void*>(new char[GetVarTypeSize(initializer_data->type)]), GetVarTypeSize(initializer_data->type) });
		}

	}

private:
	std::vector<VariableValue> value;
	VarType type = VarType::VT_INVALID;
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

inline bool ValidDeclarationOperator(const std::string_view& op)
{
	if (op.empty()) //should never be true
		return false;

	auto& o = op.front();

	return o == '?' || o == '[' || o == ']';

}

enum class declr_type : char
{
	DEFAULT,
	REFERENCE,
	ARRAY
};

struct var_declr_data
{
	std::shared_ptr<array_declr_data> arr;
	declr_type declaration_type;
	std::string variable_name;
	std::string variable_type;
};

declr_type DeclarationUnaryToType(char op);

void DeclareVariable(const var_declr_data& data);
#endif