
#ifndef _rval_
#define _rval_

#include "pch.hpp"
#include "script.hpp"
#include "datatype.hpp"


struct VariableValue
{
	std::shared_ptr<char*> buffer = 0;
	size_t buf_size = 0;

};

struct rval_declr_data
{
	dataTypes_e type = dataTypes_e::UNKNOWN;
	size_t size = 0ull; //optional unless the datatype is not a constant size
	const token_t* token = 0;
};

struct rvalue
{
	rvalue(const rval_declr_data& declaration_data);

	char get_char() const { return get_value<char>(); }
	int get_int() const { return get_value<int>(); }
	float get_float() const { return get_value<float>(); }

	template<typename T> T get_value() const
	{
		if (value.buffer.use_count() == NULL) { throw scriptError_t("rvalue: called get_value() without a value.. how?"); }
		return *reinterpret_cast<T*>(value.buffer.get());
	}

	template<typename T> void set_value(const T& _value)
	{
		if (value.buffer.use_count() == NULL) { throw scriptError_t("rvalue: called set_value() without a value.. how?"); }
		*reinterpret_cast<T*>(value.buffer.get()) = _value;
	}
	void set_initial_value(const std::string& s);
	void replace_value(const VariableValue& v) 
	{ 


		value.buffer.reset(); 
		value.buffer = std::make_shared<char*>(new char[v.buf_size]);
		value.buf_size = v.buf_size;

		memcpy(value.buffer.get(), v.buffer.get(), v.buf_size);

	}


	auto get_type() const noexcept { return type; }
	void set_type(const dataTypes_e d) noexcept { type = d; }
	auto get_code_pos() const noexcept(true) { return std::make_pair(token.line, token.column); }
	VariableValue value;
	token_t token; //read_only
private:
	
	
	dataTypes_e type;
};

#endif