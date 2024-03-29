
#include "expression_token.hpp"
#include "variable.hpp"
#include "o_unary.hpp"
#include "o_postfix.hpp"
#include "o_standard.hpp"

void expression_token::set_value_category()
{
	if (op)
		return;

	if (is_lvalue() || is_rvalue())
		return;

	if(token.tt >= tokenType::BUILT_IN_TYPE)
		throw scriptError_t(&token, std::format("expected an expression, but found \"{}\"", token.string));

	

	if (token.tt == tokenType::NAME) {

		auto function_table = block->get_function_table();
		auto func = function_table->find(token.string);

		if (func != function_table->end()) {
			function = &func->second;
			return;
		}

		auto result = block->find_variable(token.string);

		if (result == nullptr) {
			throw scriptError_t(&token, std::format("the identifier \"{}\" is undefined", token.string));
		}
		
		lval = result;
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


using UnaryFunctionType = std::function<void(expression_token&)>;
using OptionalUnaryFunctionType = std::optional<UnaryFunctionType>;

void expression_token::eval_postfix(scr_scope_t* _block)
{

	if (postfix.empty())
		return;

	using PostfixFunctionType = std::function<void(scr_scope_t*, expression_token&, postfixBase*)>;
	using OptionalPostfixFunctionType = std::optional<PostfixFunctionType>;

	auto& front = postfix.front();
	
	auto& postfixInstance = postfixFunctions::getInstance();	
	const OptionalPostfixFunctionType function = postfixInstance.find_function(front->getType());

	if (!function.has_value()) {
		throw scriptError_t(&token, std::format("no function for the {} postfix operator???????", static_cast<int>(front->getType())));
	}

	function.value()(_block, *this, front.get());

	postfix.pop_front();
	eval_postfix(_block);
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

	const OptionalUnaryFunctionType function = unaryInstance.find_function(punctuation);
	
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
		//LOG("no implicit cast!\n";
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

	other.cast_to_type(get_type());
}
void expression_token::cast_to_type(const dataTypes_e target)
{
	switch (target) {
	case dataTypes_e::INT:
		LOG("implicitly casting to 'int'\n");
		//set_value<int>(implicit_cast<int>());
		rval->set_type(dataTypes_e::INT);
		break;
	case dataTypes_e::FLOAT:
		LOG("implicitly casting to 'float'\n");
		//set_value<float>(implicit_cast<float>());
		rval->set_type(dataTypes_e::FLOAT);
		break;
	}
}
void expression_token::implicit_force_cast(const datatype_declaration& target)
{
	if (is_compatible_with_type(target) == false) {
		throw scriptError_t(&token, std::format("cannot implicitly cast from \"{}\" to \"{}\"", get_datatype().get_as_text(), target.get_as_text()).c_str());
	}
	
	

	if (is_lvalue()) {

		if (lval->is_array()) { //no cast needed
			return;
		}

		lvalue_to_rvalue();
	}


	cast_to_type(target.dtype);

}
datatype_declaration expression_token::get_datatype() const noexcept
{
	datatype_declaration type;

	type.dtype = get_type();

	if (is_rvalue() || lval->is_array() == false)
		return type;

	for (size_t i = 0; i < lval->array_depth(); i++)
		type.typeModifiers.push_back(declaration_modifiers_e::ARRAY);

	return type;
}
template<typename T>
T expression_token::implicit_cast() const {
	//if (typeid(T) == typeid(int)) {
	//	switch (get_type()) {
	//	case dataTypes_e::CHAR:
	//		return static_cast<int>(get_char());
	//	case dataTypes_e::INT:
	//		return get_int();
	//	case dataTypes_e::FLOAT:
	//		return static_cast<int>(get_float());
	//	default:
	//		throw std::exception("this cast is illegal - this should never execute");
	//	}
	//}
	//else if (typeid(T) == typeid(float)) {
	//	switch (get_type()) {
	//	case dataTypes_e::CHAR:
	//		return static_cast<float>(get_char());
	//	case dataTypes_e::INT:
	//		return static_cast<float>(get_int());
	//	case dataTypes_e::FLOAT:
	//		return get_float();
	//	default:
	//		throw std::exception("this cast is illegal - this should never execute");
	//	}
	//}
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
			LOG("a char gets promoted to an int\n");

			weaker->rval->set_value<int>(weaker->implicit_cast<int>());
			weaker->rval->set_type(dataTypes_e::INT);
			break;
		}

		break;
	case dataTypes_e::FLOAT:
		

		switch (weaker->get_type()) {
		case dataTypes_e::CHAR:
			LOG("a char gets promoted to a float\n");

			weaker->rval->set_value<float>(weaker->implicit_cast<float>());
			weaker->rval->set_type(dataTypes_e::FLOAT);
			break;

		case dataTypes_e::INT:
			LOG("an int gets promoted to a float\n");

			weaker->rval->set_value<float>(weaker->implicit_cast<float>());
			weaker->rval->set_type(dataTypes_e::FLOAT);
			break;
		default:
			break;
		}
		break;

	}
}
void expression_token::test_operand_compatibility(const expression_token& other) const
{
	if (compatible_operand(other))
		return;

	const auto left_type = is_lvalue() ? lval->s_getvariabletype() : get_type_as_text(get_type());
	const auto right_type = other.is_lvalue() ? other.lval->s_getvariabletype() : get_type_as_text(other.get_type());

	throw scriptError_t(&token, std::format("an operand of type \"{}\" is incompatible with \"{}\"", left_type, right_type).c_str());
	
}
bool expression_token::is_compatible_with_type(const datatype_declaration& other) const noexcept
{
	const auto thistype = get_datatype();

	if (thistype.has_same_modifiers(other) == false) {
		return false;
	}

	//needs to be handled when new types are added

	return true;

}
bool expression_token::compatible_operand(const expression_token& other) const
{
	
	if (is_lvalue()) {

		if (compatible_array_operand(other))
			return true;

		return false;

	}

	return true;

}
bool expression_token::compatible_array_operand(const expression_token& other) const
{
	const size_t ldepth = array_depth();
	const size_t rdepth = other.array_depth();

	if (!ldepth && !rdepth)
		return true;

	if (ldepth != rdepth)
		return false;

	if (get_type() != other.get_type())
		return false;


	return true;
}