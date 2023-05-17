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
	std::shared_ptr<char*> buffer = 0;
	unsigned int buf_size = 0; 
};


struct array_declr_data
{
	int numElements = 0;
	VarType type;
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
			value.push_back({ std::make_shared<char*>(new char[GetVarTypeSize(initializer_data->type)]), GetVarTypeSize(initializer_data->type) });
		}

	}

	std::shared_ptr<Array> child;

private:
	std::vector<VariableValue> value;
	VarType type = VarType::VT_INVALID;
};

class Variable
{
public:
	Variable(const std::string_view& _name, VarType _type);
	Variable(VarType _type) : type(_type) {}
	Variable() {};
	~Variable();

	int get_int() const		 { return *reinterpret_cast<int*>(value.buffer.get());	}
	float get_float() const  { return *reinterpret_cast<float*>(value.buffer.get());}
	char* get_string() const { return reinterpret_cast<char*>(value.buffer.get());	}

	void set_int(int _value)		{*reinterpret_cast<int*>(value.buffer.get()) = _value; }
	void set_float(float _value)	{*reinterpret_cast<float*>(value.buffer.get()) = _value; }
	void set_string(char* str) {
		const auto len = strlen(str);
		//str[len - 1] = '\0';

		if (len != value.buf_size) {
			value.buffer.reset();
			value.buffer = std::make_shared<char*>(new char[len + 1]);
			char* alloc = (char*)value.buffer.get();
			alloc[len] = '\0';

		}
		memcpy(value.buffer.get(), str, len);
		value.buf_size = strlen(get_string()); //a bit more expensive but works surely :clueless:
	}

	template <typename T>
	void set_value(const T v) { *reinterpret_cast<T*>(value.buffer.get()) = v; }
	void set_type(const VarType atype) { type = atype; }
	auto get_type() const { return type; }
	
	void AllocateValues();
	std::string name;
	VariableValue value;
	friend class Array;

	//references
	std::shared_ptr<Variable> reference;

	//reference methods
	bool is_reference() const { return reference.get(); }

	//arrays
	std::shared_ptr<Variable[]> arr;
	unsigned __int16 numElements = 0;

	//array methods
	bool is_array() const { return arr.get(); }
	void replace_array(const std::shared_ptr<Variable[]>& a_arr, const unsigned __int16 length);

	//utility functions
	void print(unsigned __int16 spaces = 0) const;
	std::string s_getvariabletype() const;

private:
	
	VarType type = VarType::VT_INVALID;
	
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
VarType GetDataType(const std::string_view& str);

enum class declr_type : char
{
	DEFAULT,
	REFERENCE,
	ARRAY
};

struct var_declr_data
{
	std::shared_ptr<array_declr_data> arr;
	declr_type declaration_type = declr_type::DEFAULT;
	bool reference = false;
};

declr_type DeclarationUnaryToType(char op);

Variable* DeclareVariable(const std::string& name, const VarType type);
unsigned __int16 GetArrayDepth(const Variable* var);
std::string S_GetVariableType(const Variable& var);


#endif