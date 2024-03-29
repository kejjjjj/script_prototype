#include "variable.hpp"
#include "expression_token.hpp"
#include "o_standard.hpp"
#include "r_expression_token.hpp"

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

	arrayElements.resize(numElements);
	arrayElements[0] = std::unique_ptr<Variable>((new Variable()));
	arrayElements[0]->set_type(get_type());
	arrayElements[0]->AllocateValues();
	arrayElements[0]->isInitialized = true;



}
void Variable::resize_array(const size_t newSize)
{
	if (!is_array())
		throw scriptError_t("why are we trying to resize a non-array?");

	if (newSize == numElements)
		return;

	if (newSize <= numElements) {
		arrayElements.erase(arrayElements.begin()+newSize, arrayElements.end());
		return;
	}

	bool childArray = arrayElements[0]->is_array();

	//std::vector<std::unique_ptr<Variable>> newArray(newSize);

	//const size_t copySize = std::min(newSize, numElements);

	//for (size_t i = 0; i < copySize; ++i) {
	//	newArray[i] = std::move(arrayElements[i]);
	//}

	//arrayElements = std::move(newArray);

	const size_t iterations = newSize - numElements;

	for (size_t i = 0; i < iterations; i++) {
		arrayElements.push_back(std::unique_ptr<Variable>(new Variable()));

		auto& back = arrayElements.back();

		back->set_type(get_type());
		back->AllocateValues();
		back->isInitialized = true;

		if (childArray)
			back->create_array();
	}
	
	numElements = newSize;

}
void Variable::set_array_depth(const size_t newSize)
{
	if (!is_array())
		create_array();

	for (size_t i = 0; i < numElements; i++) {

		if (arrayElements[i]->array_depth() != newSize - 1ull) {
			arrayElements[i]->set_array_depth(newSize - 1ull);
		}

		
	}
}
void Variable::replace_array(const std::vector<std::unique_ptr<Variable>>& a_arr, const size_t length)
{
	std::cout << "replacing array\n";
	if (length != numElements)
		resize_array(length);

	const auto f = evaluationFunctions::getInstance().find_function(P_ASSIGN);
	expression_token l, r;


	if (!f.has_value())
		throw scriptError_t("how?");

	for (size_t i = 0; i < length; i++) {
		l = arrayElements[i]->to_expression();
		r = std::move(a_arr[i]->to_expression());
		f.value()(l, r);
	}

	numElements = length;
}
size_t Variable::array_depth() const
{
	if (!is_array())
		return 0;

	Variable* a = arrayElements[0].get();
	size_t size{ 0 };

	while (a) {
		++size;

		if (!a->is_array())
			break;

		a = a->arrayElements[0].get();
	}

	return size;
}
void Variable::print(size_t spaces) const
{

	if (!spaces++ && is_array()) {
		std::cout << std::format("{}:\n", identifier);
	}

	auto getval = [](const Variable* var) -> std::string
	{
		std::string c;
		switch (var->type) {
		case dataTypes_e::CHAR:
			 return c.push_back(var->get_char()), c;
		case dataTypes_e::INT:
			return std::to_string(var->get_int());
		case dataTypes_e::FLOAT:
			return std::to_string(var->get_float());

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
		if (arrayElements[i]->is_array() == false) {
			std::cout << std::format("{}[{}]: <{}> ({})\n", prefix, i, get_type_as_text(type), getval(arrayElements[i].get()));
		}
		else {
			std::cout << std::format("{}[{}]: <{}>\n", prefix, i, get_type_as_text(type));

		}
		arrayElements[i]->print(spaces + 1);
	}
	if (is_array() == false && !identifier.empty())
		std::cout << std::format("{}: <{}> ({})\n", identifier, get_type_as_text(type), getval(this));

//	std::cout << std::format("{}{}: <{}> ({})\n", prefix, identifier, get_type_as_text(type), getval(*this));



}
std::string Variable::s_getvariabletype() const
{
	std::function<std::string(const Variable*)> types_to_text = [&types_to_text](const Variable* var) -> std::string {

		if (!var)
			return "";

		if (var->is_array())
			return "[]" + types_to_text(var->arrayElements[0].get());

		return "";

	};

	return std::format("{}{}", get_type_as_text(get_type()), types_to_text(this));

}