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
		arrayElements[i].isInitialized = true;

	}



}
void Variable::resize_array(const size_t newSize)
{
	if(!is_array())
		throw scriptError_t("why are we trying to resize a non-array?");

	if (newSize == numElements)
		return;

	bool childArray = arrayElements[0].is_array();

	std::shared_ptr<Variable[]> newArray(new Variable[newSize]);

	const size_t copySize = std::min(newSize, numElements);
	std::copy(arrayElements.get(), arrayElements.get() + copySize, newArray.get());

	arrayElements = newArray;
	//std::cout << "resized the array from size " << numElements << " to " << newSize << '\n';
	numElements = newSize;

	for (decltype(numElements) i = copySize; i < newSize; i++) {
		arrayElements[i].set_type(get_type());
		arrayElements[i].AllocateValues();
		arrayElements[i].isInitialized = true;

		if (childArray)
			arrayElements[i].create_array();
	}
}
void Variable::replace_array(const std::shared_ptr<Variable[]>& a_arr, const size_t length)
{
	if (length != numElements)
		std::cout << "resizing the array from " << numElements << " to " << length << '\n';

	arrayElements = a_arr;
	numElements = length;
}
void Variable::print(size_t spaces) const
{

	if (!spaces++) {
		std::cout << std::format("{}:\n", identifier);
	}

	auto getval = [](const Variable& var) -> std::string
	{
		std::string c;
		switch (var.type) {
		case dataTypes_e::CHAR:
			 return c.push_back(var.get_char()), c;
		case dataTypes_e::INT:
			return std::to_string(var.get_int());
		case dataTypes_e::FLOAT:
			return std::to_string(var.get_float());

		default:
			return "null";
		}
	};	

	std::string prefix;

	for (size_t i = 0ull; i < spaces; i++) {
		prefix.push_back(' ');
		prefix.push_back(' ');

	}

	for (size_t i = 0; i < numElements; i++) {
		std::cout << std::format("{}[{}]: <{}> ({})\n", prefix, i, get_type_as_text(type), getval(arrayElements[i]));
		arrayElements[i].print(spaces + 1);
	}
	if (!numElements && !identifier.empty())
		std::cout << std::format("{}: <{}> ({})\n", prefix, get_type_as_text(type), getval(*this));

//	std::cout << std::format("{}{}: <{}> ({})\n", prefix, identifier, get_type_as_text(type), getval(*this));



}
std::string Variable::s_getvariabletype() const
{
	std::function<std::string(const Variable*)> types_to_text = [&types_to_text](const Variable* var) -> std::string {

		if (!var)
			return "";

		if (var->is_array())
			return "[]" + types_to_text(var->arrayElements.get());

		return "";

	};

	return std::format("{}{}", get_type_as_text(get_type()), types_to_text(this));

}