
#include "expression_token.hpp"
#include "variable.hpp"
#include "o_unary.hpp"

void expression_token::set_value_category()
{
	if (op)
		return;

	if (is_lvalue() || is_rvalue())
		return;

	if(token.tt == tokenType::BUILT_IN_TYPE)
		throw scriptError_t(&token, std::format("a datatype was unexpected here"));

	if (token.tt == tokenType::NAME) {
		auto& instance = VariableTable::getInstance();
		auto result = instance.find(token.string);

		if (result.has_value() == false) {
			throw scriptError_t(&token, std::format("the identifier \"{}\" is undefined", token.string));
		}
		
		lval = &result.value()->second;
		return;

	}

	//passing &token is fine because the rvalue's lifetime is the same as this expression_token :clueless:
	
	switch (token.tt) {
	case tokenType::CHAR_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({
			.type = dataTypes_e::CHAR,
			.token = &token }));
		break;
	case tokenType::NUMBER_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({
			.type = typeFromIntegerSuffix(token.extrainfo),
			.token = &token }));

		break;
	case tokenType::FLOAT_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({
			.type = dataTypes_e::FLOAT,
			.token = &token }));
		break;
	default:
		throw scriptError_t(&token, "this value type is not supported yet :)");

	}
	rval->set_initial_value(token.string);

}

using FunctionType = std::function<void(expression_token&)>;
using OptionalFunctionType = std::optional<FunctionType>;

void expression_token::eval_postfix()
{
	if (postfix.empty())
		return;

	postfix.pop_front();

	eval_postfix();
}
void expression_token::eval_prefix()
{
	if (prefix.empty())
		return;

	auto& back = prefix.back();

	if (back->tt != tokenType::PUNCTUATION) {
		throw scriptError_t(&token, "cannot evaluate unary on non-punctuation type yet! :)");
	}

	auto& unaryInstance = unaryFunctions::getInstance();
	const auto punctuation = static_cast<punctuation_e>(LOWORD(back->extrainfo));

	const OptionalFunctionType function = unaryInstance.find_function(punctuation);
	
	if (!function.has_value()) {
		throw scriptError_t(&token, std::format("no function for the \"{}\" unary operator", back->string));
	}

	function.value()(*this);

	prefix.pop_back();
	eval_prefix();
}

void expression_token::lvalue_to_rvalue()
{
	if (!is_lvalue())
		return;

	if (lval->isInitialized == false)
		throw scriptError_t(&token, std::format("the variable \"{}\" is uninitialized", lval->identifier));

	rval = std::shared_ptr<rvalue>(new rvalue({ 
			.type = lval->get_type(), 
			.size = lval->value.buf_size, 
			.token = &token 
		}));


	rval->replace_value(lval->value);


	lval = nullptr;

}
void expression_token::implicit_cast(expression_token& other)
{
	if (get_type() == other.get_type()) {
		//std::cout << "no implicit cast!\n";
		return;
	}

	if (is_lvalue() == false) {
		cast_weaker_operand(other);
		return;
	}

	if (size_of() < other.size_of()) {
		throw scriptError_t(&other.get_token(), std::format("cannot implicitly convert from \"{}\" to \"{}\" due to data loss", 
			get_type_as_text(other.get_type()),
			get_type_as_text(get_type())));
		return;
	}

	if (other.is_lvalue()) {
		throw scriptError_t("implicit_cast(): right operand should not be an lvalue");
		return;
	}

	switch (get_type()) {
	case dataTypes_e::INT:
		std::cout << "implicitly casting to 'int'\n";
		other.set_value<int>(other.implicit_cast<int>());
		other.rval->set_type(dataTypes_e::INT);
		break;
	case dataTypes_e::FLOAT:
		std::cout << "implicitly casting to 'float'\n";
		other.set_value<float>(other.implicit_cast<float>());
		other.rval->set_type(dataTypes_e::FLOAT);
		break;
	}
}

template<typename T>
T expression_token::implicit_cast() const {
	if (typeid(T) == typeid(int)) {
		switch (get_type()) {
		case dataTypes_e::CHAR:
			return static_cast<int>(get_char());
		case dataTypes_e::INT:
			return get_int();
		case dataTypes_e::FLOAT:
			return static_cast<int>(get_float());
		default:
			throw std::exception("this cast is illegal - this should never execute");
		}
	}
	else if (typeid(T) == typeid(float)) {
		switch (get_type()) {
		case dataTypes_e::CHAR:
			return static_cast<float>(get_char());
		case dataTypes_e::INT:
			return static_cast<float>(get_int());
		case dataTypes_e::FLOAT:
			return get_float();
		default:
			throw std::exception("this cast is illegal - this should never execute");
		}
	}
	throw std::exception("this cast is illegal - this should never execute");

}
void expression_token::cast_weaker_operand(expression_token& other)
{
	if (is_lvalue() || other.is_lvalue()) {
		throw scriptError_t("somehow cast_weaker_operand was called with an lvalue?");
	}

	expression_token* stronger, *weaker;

	if (get_type() > other.get_type()) {
		stronger = this;
		weaker = &other;
	}
	else if (get_type() < other.get_type()) {
		stronger = &other;
		weaker = this;
	}
	else
		return;


	switch (stronger->get_type()) {
	case dataTypes_e::INT:

		switch (weaker->get_type()) {
		case dataTypes_e::CHAR:
			std::cout << "a char gets promoted to an int\n";

			weaker->rval->set_value<int>(weaker->implicit_cast<int>());
			weaker->rval->set_type(dataTypes_e::INT);
			break;
		}

		break;
	case dataTypes_e::FLOAT:
		

		switch (weaker->get_type()) {
		case dataTypes_e::CHAR:
			std::cout << "a char gets promoted to a float\n";

			weaker->rval->set_value<float>(weaker->implicit_cast<float>());
			weaker->rval->set_type(dataTypes_e::FLOAT);
			break;

		case dataTypes_e::INT:
			std::cout << "an int gets promoted to a float\n";

			weaker->rval->set_value<float>(weaker->implicit_cast<float>());
			weaker->rval->set_type(dataTypes_e::FLOAT);
			break;
		default:
			break;
		}
		break;

	}
}
void expression_token::print() const noexcept
{
	std::cout << "\n---------TOKEN---------\n";
	for (const auto& i : prefix)
		std::cout << i->string << ' ';
	std::cout << '\n';
	std::cout << token.string << '\n';
	for (const auto& i : postfix)
		std::cout << i->string << ' ';

	std::cout << '\n';
}