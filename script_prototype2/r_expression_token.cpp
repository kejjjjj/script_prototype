#include "r_expression_token.hpp"
#include "r_postfix.hpp"
#include "r_unary.hpp"
#include "runtime.hpp"

using PostfixFunctionType = std::function<void(scr_scope_t*, expression_token&, std::optional<token_statement_t*>)>;
using OptionalPostfixFunctionType = std::optional<PostfixFunctionType>;

using UnaryFunctionType = std::function<void(expression_token&)>;
using OptionalUnaryFunctionType = std::optional<UnaryFunctionType>;

template<typename T>
T r_expression_token::implicit_cast() const {
	if (typeid(T) == typeid(int)) {
		switch (get_type()) {
		case dataTypes_e::CHAR:
			return static_cast<int>(get_char());
		case dataTypes_e::INT:
			return get_int();
		case dataTypes_e::FLOAT:
			return static_cast<int>(get_float());
		default:
			throw runtimeError_t("this cast is illegal - this should never execute");
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
			throw runtimeError_t("this cast is illegal - this should never execute");
		}
	}
	throw runtimeError_t("this cast is illegal - this should never execute");

}
void r_expression_token::implicit_cast(r_expression_token& other)
{
	if (get_type() == other.get_type()) {
		return;
	}

	if (is_lvalue() == false) {
		cast_weaker_operand(other);
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
void r_expression_token::cast_weaker_operand(r_expression_token& other)
{

	r_expression_token* stronger, *weaker;

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
void r_expression_token::lvalue_to_rvalue()
{
	if (!is_lvalue())
		return;

	if (lval->isInitialized == false)
		throw scriptError_t(&token, std::format("the variable \"{}\" is uninitialized", lval->identifier));

	rval = std::unique_ptr<rvalue>(new rvalue({
			.type = lval->get_type(),
			.size = lval->value.buf_size,
			.token = &token
		}));


	rval->replace_value(lval->value);


	lval = nullptr;

}
void r_expression_token::eval_postfix(scr_scope_t* _block)
{
	if (postfix.empty())
		return;

	//auto& front = postfix.front();

	//auto& postfixInstance = postfixFunctions::getInstance();
	//const OptionalPostfixFunctionType function = postfixInstance.find_function(front.second);

	//function.value()(block, *this, &front.first);

	postfix.pop_front();
	eval_postfix(_block);
}
void r_expression_token::eval_prefix()
{
	if (prefix.empty())
		return;

	//auto& back = prefix.back();

	//auto& unaryInstance = unaryFunctions::getInstance();
	//const auto punctuation = static_cast<punctuation_e>(LOWORD(back->extrainfo));

	//const OptionalUnaryFunctionType function = unaryInstance.find_function(punctuation);


	//function.value()(*this);

	prefix.pop_back();
	eval_prefix();
}