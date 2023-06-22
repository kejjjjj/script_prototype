#include "declaration.hpp"

void declaration_t::declare_and_initialize()
{
	get_declaration_type();
	get_identifier_name();

	std::cout << std::format("declaration of type \"{}\", with an identifier of \"{}\"\n", static_cast<int>(dtype), identifier);
}
void declaration_t::get_declaration_type()
{
	auto& instance = dataTypeTable::getInstance();

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
bool declaration_t::get_type_modifiers()
{
	return 1;
}