#pragma once

#ifndef scriptvars
#define scriptvars

#include "pch.h"

struct VariableValue
{
	std::shared_ptr<char*> buffer = 0;
	size_t buf_size = 0; 
};


struct array_declr_data
{
	int numElements = 0;
	VarType type = VarType::VT_INVALID;
};

namespace expr
{
	struct expression_token;
}
class Variable
{
public:
	Variable(const std::string_view& _name, VarType _type);
	Variable(VarType _type) : type(_type) {}
	Variable() = default;
	~Variable() = default;

	int get_int() const		 { return *reinterpret_cast<int*>	(value.buffer.get());	}
	float get_float() const  { return *reinterpret_cast<float*>	(value.buffer.get());	}
	char get_char() const { return *reinterpret_cast<char*>	(value.buffer.get()); }

	char* get_string() const { return reinterpret_cast<char*>	(value.buffer.get());	}

	void set_int(int _value)		{*reinterpret_cast<int*>	(value.buffer.get()) = _value; }
	void set_float(float _value)	{*reinterpret_cast<float*>	(value.buffer.get()) = _value; }
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
	
	void set_expression(expr::expression_token* token);

	void initialize_expression(expr::expression_token* token);
	void set_type(const VarType atype) { type = atype; }
	auto get_type() const { return type; }
	decltype(auto) this_or_ref() { return is_pointer() ? pointer.get() : this; }
	
	void AllocateValues();
	std::string name;
	VariableValue value;

	//references
	std::shared_ptr<Variable> pointer;

	//reference methods
	bool is_pointer() const { return pointer.get(); }

	//arrays
	std::shared_ptr<Variable[]> arr;
	size_t numElements = 0;

	//array methods
	bool is_array() const { return arr.get(); }
	void replace_array(const std::shared_ptr<Variable[]>& a_arr, const size_t length);
	void recreate_array(const size_t new_length);

	//utility functions
	void print(size_t spaces = 0) const;
	std::string s_getvariabletype() const;

	//operator overloads
	operator expr::expression_token();

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