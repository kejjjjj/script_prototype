#include "pch.h"


rvalue::rvalue(const VarType _type, const size_t size) : type(_type) {
	char* buf;
	switch (_type) {
	case VarType::VT_INT:
		value.buffer = std::make_shared<char*>(new char[4]);
		value.buf_size = sizeof(int);

		break;
	case VarType::VT_FLOAT:
		value.buffer = std::make_shared<char*>(new char[4]);
		value.buf_size = sizeof(float);
		break;
	case VarType::VT_STRING:
		if (!size)
			throw std::exception("empty string literal is not allowed");

		value.buffer = std::make_shared<char*>(new char[size + 1u]);
		buf = (char*)value.buffer.get();
		buf[size] = '\0';
		value.buf_size = size + 1;
		break;
	case VarType::VT_CHAR:
		value.buffer = std::make_shared<char*>(new char[1]);
		value.buf_size = sizeof(char);
		break;
	}
}

std::string rvalue::s_gettype() const
{
	std::function<std::string(const Variable*)> types_to_text = [&types_to_text](const Variable* r) -> std::string {

		if (!r)
			return "";

		if (r->is_pointer())
			return "^" + types_to_text(r->pointer.get());

		return "";

	};

	return std::format("{}{}", VarTypes[int(get_type())], types_to_text(pointer.get()));
}
size_t rvalue::pointer_depth() const
{
	Variable* p = pointer.get();
	//Variable* a = arr.get();
	decltype(pointer_depth()) size{ 0 };

	while (p /*|| a*/) {
		++size;

		if (p) {
			p = p->pointer.get();
			break;
		}

		//a = a->arr.get();
	}

	return size;
}
size_t rvalue::array_depth() const
{
	Variable* a = arr.get();
	decltype(pointer_depth()) size{ 0 };

	while (a) {
		++size;

		a = a->arr.get();
	}

	return size;
}