#include "variable.hpp"
#include "expression_token.hpp"

Variable::Variable(const var_declr_data& init_data)
{
	identifier = init_data.name;
	type = init_data.type;

	AllocateValues();
}
void Variable::AllocateValues()
{
	switch (type) {
	case dataTypes_e::CHAR:
		value.buffer = std::make_unique<char*>(new char[sizeof(char)]);
		value.buf_size = sizeof(char);
		break;
	case dataTypes_e::INT:
		value.buffer = std::make_unique<char*>(new char[sizeof(int32_t)]);
		value.buf_size = sizeof(int32_t);
		break;
	case dataTypes_e::FLOAT:
		value.buffer = std::make_unique<char*>(new char[sizeof(float)]);
		value.buf_size = sizeof(float);
		break;

	default:
		throw scriptError_t("huh.. variable constructed without a type..");

	}
}
expression_token Variable::to_expression()
{
	expression_token token;
	
	token.lval = this;
	token.op = false;

	return token;
}
void Variable::create_array()
{
	numElements = 1;
	arrayElements = std::shared_ptr<Variable[]>(new Variable[numElements]);

	for (decltype(numElements) i = 0; i < numElements; i++) {
		arrayElements[i].set_type(get_type());
		arrayElements[i].AllocateValues();

	}
}
void Variable::resize_array(const size_t newSize)
{
	if(!is_array())
		throw scriptError_t("why are we trying to resize a non-array?");

	std::shared_ptr<Variable[]> newArray(new Variable[newSize]);

	const size_t copySize = std::min(newSize, numElements);
	std::copy(arrayElements.get(), arrayElements.get() + copySize, newArray.get());

	arrayElements = newArray;
	std::cout << "resized the array from size " << numElements << " to " << newSize << '\n';
	numElements = newSize;

}
void Variable::print(size_t spaces) const
{
	auto getval = [this]() -> std::string
	{
		std::string c;
		switch (type) {
		case dataTypes_e::CHAR:
			 return c.push_back(get_char()), c;
		case dataTypes_e::INT:
			return std::to_string(get_int());
		case dataTypes_e::FLOAT:
			return std::to_string(get_float());

		default:
			return "null";
		}
	};	

	std::string prefix;

	for (size_t i = 0ull; i < spaces; i++) {
		prefix.push_back(' ');
		prefix.push_back(' ');

	}

	std::cout << std::format("{}{}: <{}> ({})\n", prefix, identifier, get_type_as_text(type), getval());



}