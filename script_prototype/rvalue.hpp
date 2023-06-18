
#ifndef _rval_
#define _rval_

#include "pch.h"

struct rvalue
{
	rvalue(const VarType _type, const size_t size = 0);
	//~rvalue();
	rvalue() = delete;
	VariableValue value;

	std::shared_ptr<Variable> pointer;
	
	//arrays
	std::shared_ptr<Variable[]> arr;
	size_t numElements = 0;

	//array methods
	bool is_array() const { return arr.get(); }

private:

	VarType type;
public:
	bool is_pointer() const {
		return pointer.get();
	}
	int get_int() const {
		return *reinterpret_cast<int*>(value.buffer.get());
	}
	float get_float() const {
		return *reinterpret_cast<float*>(value.buffer.get());
	}
	double get_double() const {
		return *reinterpret_cast<double*>(value.buffer.get());
	}
	char* get_string() const {
		return reinterpret_cast<char*>(value.buffer.get());
	}
	char get_char() const {
		return *reinterpret_cast<char*>(value.buffer.get());
	}
	template <typename T>
	void set_value(const T v) {
		*reinterpret_cast<T*>(value.buffer.get()) = v;
	}
	void set_string(char* str) {
		const auto len = strlen(str);

		if (len != value.buf_size) {
			value.buffer.reset();
			value.buffer = std::make_shared<char*>(new char[len + 1]);
			char* alloc = (char*)value.buffer.get();
			alloc[len] = '\0';

		}
		memcpy(value.buffer.get(), str, len);
		value.buf_size = strlen(get_string()); //a bit more expensive but works surely :clueless:
	}
	auto get_type() const {
		return type;

	}
	std::string s_gettype() const;
	size_t pointer_depth() const;
	size_t array_depth() const;

	void set_type(const VarType atype) {
		type = atype;
	}

};

#endif