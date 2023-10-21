#include "declaration.hpp"
#include "variable.hpp"
#include "expression.hpp"
#include "initializer_list.hpp"

variable_declaration_t::variable_declaration_t(scr_scope_t* scope, const code_segment_t& expression) : tokens(expression), block(scope)
{
	tokens.end++;
	//compiler_declr = std::unique_ptr<variable_declaration_t_compiler>(new variable_declaration_t_compiler);
};

void variable_declaration_t::declare_and_initialize(bool initializer_allowed)
{
	get_variable_declaration_type();
	get_identifier_name();
		
	target = block->declare_variable(std::move(std::unique_ptr<Variable>(new Variable(var_declr_data{ dtype, identifier, NULL }))));

	apply_modifiers(target);

	LOG("declaring a variable of type \"" << target->s_getvariabletype() << "\" with the name of \"" << target->identifier << "\"\n");

	//compiler_declr->identifier = target->identifier;

	if (has_initializer(initializer_allowed)) {

		//compiler_declr->has_initializer = true;
		initialize();

	}

}
void variable_declaration_t::get_variable_declaration_type()
{
	if (tokens.it->tt != tokenType::BUILT_IN_TYPE) {
		throw scriptError_t(&*tokens.it, "how?");
	}

	dtype = static_cast<dataTypes_e>(tokens.it->extrainfo);

	tokens.it++;
	//check for type modifiers
	get_type_modifiers();

	//std::copy(typeModifiers.begin(), typeModifiers.end(), compiler_declr->typeModifiers.begin());
	//compiler_declr->dtype = dtype;
}
void variable_declaration_t::get_identifier_name()
{

	if (tokens.it == tokens.end)
		throw scriptError_t(&*tokens.it, "expected an identifier");

	if(tokens.it->tt != tokenType::NAME)
		throw scriptError_t(&*tokens.it, "expected an identifier");

	identifier = tokens.it->string;

	if (block->find_variable(identifier)) {
		throw scriptError_t(&*tokens.it, std::format("the variable \"{}\" is already defined", identifier));
	}
}
bool variable_declaration_t::has_initializer(bool initializer_allowed)
{
	++tokens.it;

	if (tokens.it == tokens.end)
		return false;

	if (initializer_allowed == false) {
		throw scriptError_t(&*tokens.it, std::format("expected for the declaration to end, but found \"{}\"", tokens.it->string));
	}

	if (tokens.it->tt != tokenType::PUNCTUATION || LOWORD(tokens.it->extrainfo) != punctuation_e::P_ASSIGN) {
		throw scriptError_t(&*tokens.it, "expected a \"=\" or \";\"");
		return false;
	}

	return true;
}
void variable_declaration_t::initialize()
{
	++tokens.it;

	const auto is_initializer_list = [](const token_t& token) { return (token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN); };


	//handle initializer list
	if(is_initializer_list(*tokens.it))
	{
		//compiler_declr->initializer_list = true;
		if (auto substr = initializer_list_t::find_curlybracket_substring(tokens)) {
			tokens.it++; //ignore first {
			tokens.end = --(substr.value().end); //ignore last }
		}

		initializer_list_t ilist(block, tokens, target);
		ilist.evaluate();
		return;
	}
	tokens.end--; //remove the ;
	auto result = expression_t(block, tokens);
	auto value = result.EvaluateEntireExpression();

	const auto f = evaluationFunctions::getInstance().find_function(P_ASSIGN);

	if (!f.has_value())
		throw scriptError_t(&*tokens.it, "how?");
	
	auto e = target->to_expression();
	e.set_token(value.get_token());
	f.value()(e, value);

}
void variable_declaration_t::get_type_modifiers()
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

bool variable_declaration_t::parse_subscript()
{
	const auto is_opening = [](const token_t& token) { return (token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_OPEN); };
	const auto is_closing = [](const token_t& token) { return (token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_CLOSE); };

	if (is_opening(*tokens.it) == false)
		return 0;

	tokens.it++;

	if (is_closing(*tokens.it) == false)
		throw scriptError_t(&*tokens.it, "expected a \"]\"");

	LOG("adding [] type modifier!\n");
	typeModifiers.push_back(declaration_modifiers_e::ARRAY);
	tokens.it++;
	return true;

}

void variable_declaration_t::apply_modifiers(Variable* _target)
{
	if (typeModifiers.empty()) {
		return;
	}

	auto modifier = typeModifiers.front();
	typeModifiers.pop_front();

	switch (modifier) {
	case declaration_modifiers_e::ARRAY:
		_target->create_array(); //create an array of size 1
		return apply_modifiers(_target->arrayElements[0].get()); //and then do this to every child
	default:
		throw scriptError_t("this modifier case should never execute!");
	}

	
	

}