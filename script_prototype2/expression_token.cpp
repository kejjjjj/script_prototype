
#include "expression_token.hpp"
#include "variable.hpp"

void expression_token::set_value_category()
{
	if (op)
		return;

	if(token.tt == tokenType::BUILT_IN_TYPE)
		throw scriptError_t(&token, std::format("a datatype was unexpected here"));

	if (token.tt == tokenType::NAME) {
		const auto& instance = VariableTable::getInstance();

		if (instance.find(token.string).has_value() == false) {
			throw scriptError_t(&token, std::format("the identifier \"{}\" is undefined", token.string));
		}
		throw scriptError_t(&token, std::format("yippee!"));

	}

	//passing &token is fine because the rvalue's lifetime is the same as this expression_token :clueless:

	switch (token.tt) {
	case tokenType::NUMBER_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({
			.type=dataTypes_e::INT, 
			.token=&token}));
		rval->set_value<int32_t>(std::stoi(token.string));
		return;
	case tokenType::FLOAT_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({ 
			.type = dataTypes_e::FLOAT, 
			.token = &token }));
		rval->set_value<float>(std::stof(token.string));
		return;
	default:
		throw scriptError_t(&token, "this value type is not supported yet :)");

	}

}
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

	prefix.pop_back();
	eval_prefix();
}

void expression_token::lvalue_to_rvalue()
{
	if (!is_lvalue())
		return;

	rval = std::shared_ptr<rvalue>(new rvalue({ 
			.type = lval->get_type(), 
			.size = lval->get_value().buf_size, 
			.token = &token 
		}));

	lval = nullptr;

}
void expression_token::implicit_cast(expression_token& other)
{
	if (get_type() == other.get_type())
		return;

	if (is_lvalue() == false) {
		cast_weaker_operand(other);
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
		break;
	case dataTypes_e::FLOAT:
		const auto old_type = weaker->get_type();

		switch (old_type) {
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