
#ifndef _rval_
#define _rval_

#include "pch.hpp"
#include "script.hpp"

inline std::array<const char*, 3> s_dataTypes = {
	"NULL",
	"int",
	"float"
};
enum class dataTypes_e
{
	UNKNOWN,
	INT,
	FLOAT
};

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

	template<typename T> T get_value() const
	{
		if (value.buffer.use_count() == NULL) { throw scriptError_t("rvalue: called get_value() without a value.. how?"); }
		return *reinterpret_cast<T*>(*value.buffer.get());
	}

	template<typename T> void set_value(const T& _value)
	{
		if (value.buffer.use_count() == NULL) { throw scriptError_t("rvalue: called set_value() without a value.. how?"); }
		*reinterpret_cast<T*>(*value.buffer.get()) = _value;
	}


	auto get_type() const noexcept { return type; }

private:
	const token_t* token = 0; //read_only
	VariableValue value;
	dataTypes_e type;
};

#endif