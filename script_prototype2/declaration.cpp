#include "declaration.hpp"
#include "variable.hpp"
#include "expression.hpp"
#include "initializer_list.hpp"
void declaration_t::declare_and_initialize()
{
	get_declaration_type();
	get_identifier_name();
		
	target = VariableTable::getInstance().declare_variable(var_declr_data({ dtype, identifier, NULL }));

	apply_modifiers(*target);

	std::cout << "declaring a variable of type \"" << target->s_getvariabletype() << "\" with the name of \"" << target->identifier << "\"\n";


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

	tokens.it++;
	//check for type modifiers
	get_type_modifiers();

}
void declaration_t::get_identifier_name()
{

	if (tokens.it == tokens.end)
		throw scriptError_t(&*tokens.it, "expected an identifier");

	if(tokens.it->tt != tokenType::NAME)
		throw scriptError_t(&*tokens.it, "expected an identifier");

	identifier = tokens.it->string;

	if (VariableTable::getInstance().find(identifier)) {
		throw scriptError_t(&*tokens.it, std::format("the variable \"{}\" is already defined", identifier));
	}
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

	const auto is_initializer_list = [](const token_t& token) { return (token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN); };


	//handle initializer list
	if(is_initializer_list(*tokens.it))
	{
		if (auto substr = initializer_list_t::find_curlybracket_substring(tokens)) {
			tokens.it++; //ignore first {
			tokens.end = --(substr.value().end); //ignore last }
		}

		initializer_list_t ilist(tokens, *target);
		ilist.evaluate();
		return;
	}
	tokens.end--; //remove the ;
	auto result = expression_t(tokens);
	auto value = result.EvaluateEntireExpression();
	const auto f = evaluationFunctions::getInstance().find_function(P_ASSIGN);

	if (!f.has_value())
		throw scriptError_t(&*tokens.it, "how?");
	
	auto e = target->to_expression();
	e.set_token(value.get_token());
	f.value()(e, value);

}
void declaration_t::get_type_modifiers()
{
	while (tokens.it != tokens.end) {
		if (parse_subscript()) {
			continue;
		}

		if (tokens.it->tt == tokenType::PUNCTUATION)
			throw scriptError_t(&*tokens.it, std::format("unrecognized type modifier \"{}\"", tokens.it->string));

		break;
	}

	
	
	return;
}

bool declaration_t::parse_subscript()
{
	const auto is_opening = [](const token_t& token) { return (token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_OPEN); };
	const auto is_closing = [](const token_t& token) { return (token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_CLOSE); };

	if (is_opening(*tokens.it) == false)
		return 0;

	tokens.it++;

	if (is_closing(*tokens.it) == false)
		throw scriptError_t(&*tokens.it, "expected a \"]\"");

	std::cout << "adding [] type modifier!\n";
	typeModifiers.push_back(declaration_modifiers_e::ARRAY);
	tokens.it++;
	return true;

}

void declaration_t::apply_modifiers(Variable& _target)
{
	if (typeModifiers.empty()) {
		return;
	}

	auto modifier = typeModifiers.front();
	typeModifiers.pop_front();

	switch (modifier) {
	case declaration_modifiers_e::ARRAY:
		_target.create_array(); //create an array of size 1
		return apply_modifiers(_target.arrayElements[0]); //and then do this to every child
	default:
		throw scriptError_t("this modifier case should never execute!");
	}

	
	

}