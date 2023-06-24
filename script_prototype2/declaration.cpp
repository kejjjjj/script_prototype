#include "declaration.hpp"
#include "variable.hpp"
#include "expression.hpp"
void declaration_t::declare_and_initialize()
{
	get_declaration_type();
	get_identifier_name();
		
	target = VariableTable::getInstance().declare_variable(var_declr_data({ dtype, identifier, NULL }));

	if (has_initializer()) {
		initialize();

	}
}
void declaration_t::get_declaration_type()
{
	//auto& instance = dataTypeTable::getInstance();

	if (tokens.it->tt != tokenType::BUILT_IN_TYPE) {
		throw scriptError_t(&*tokens.it, "how?");
	}

	dtype = static_cast<dataTypes_e>(tokens.it->extrainfo);

	//check for type modifiers
	get_type_modifiers();

}
void declaration_t::get_identifier_name()
{
	++tokens.it;

	if (tokens.it == tokens.end)
		throw scriptError_t(&*tokens.it, "expected an identifier");

	if(tokens.it->tt != tokenType::NAME)
		throw scriptError_t(&*tokens.it, "expected an identifier");

	identifier = tokens.it->string;
}
bool declaration_t::has_initializer()
{
	++tokens.it;

	if (tokens.it == tokens.end)
		return false;

	if (tokens.it->tt != tokenType::PUNCTUATION || LOWORD(tokens.it->extrainfo) != punctuation_e::P_ASSIGN) {
		throw scriptError_t(&*tokens.it, "expected a \"=\" or \";\"");
		return false;
	}

	return true;
}
void declaration_t::initialize()
{
	++tokens.it;

	//handle initializer list
	{

	}

	auto result = expression_t(tokens);

	if (!result.is_ready()) {
		throw scriptError_t(&*tokens.it, "WTF!");
	}

	auto value = result.EvaluateEntireExpression();
	const auto f = evaluationFunctions::getInstance().find_function(P_ASSIGN);

	if (!f.has_value())
		throw scriptError_t(&*tokens.it, "how?");

	auto e = target->to_expression();
	f.value()(e, value);

}
bool declaration_t::get_type_modifiers()
{
	return 1;
}