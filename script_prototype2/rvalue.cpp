#include "rvalue.hpp"

rvalue::rvalue(const rval_declr_data& declaration_data)
{
	type = declaration_data.type;
	token = declaration_data.token;

	switch (type) {

	case dataTypes_e::INT:
		value.buffer = std::make_unique<char*>(new char[sizeof(int32_t)]);
		value.buf_size = sizeof(int32_t);
		break;
	case dataTypes_e::FLOAT:
		value.buffer = std::make_unique<char*>(new char[sizeof(float)]);
		value.buf_size = sizeof(float);
		break;

	default:
		throw scriptError_t("huh.. rvalue constructed without a type..");

	}
}