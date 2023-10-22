#include "functions.hpp"
#include "expression.hpp"
#include "declaration.hpp"

void function_c::parse_declaration(script_t& script)
{
	if (scope->is_global_scope() == false)
		throw scriptError_t(&*code.it, "a function declaration is only allowed in the global scope");

	scope = create_scope_without_range(scope);

	auto& it = code.it;
	auto end = code.end;

	++it; //skip the fn keyword

	if (it->tt != tokenType::NAME) {
		throw scriptError_t(&*it, std::format("expected an identifier, found \"{}\"", it->string));
	}

	identifier = it->string;

	++it; //skip identifier

	if (it->tt != tokenType::PUNCTUATION || (LOWORD(it->extrainfo) != P_PAR_OPEN)) {
		throw scriptError_t(&*it, std::format("expected a \"(\""));
	}

	if (auto args_opt = expression_t::find_matching_parenthesis(code_segment_t{ .it = code.it, .begin = code.it, .end = (++code.end--) })) {
		auto& args = args_opt.value();

		args.print();

		numArgs = parse_parameters(args, 1);

		LOG("function with " << numArgs << " parameters!\n");

		it = ++args.end;
	}
	else {
		LOG("no args!\n");
		++it;
	}

	++it; //skip )
	code.end = end;
	parse_returntype(code);

	create_function(script, code);

	LOG("\n--- function info ---\n");
	LOG("name: " << identifier << '\n');
	LOG("params: (");
	for (auto it = param_types.begin(); it != param_types.end(); ++it) {
		
		auto next = it;
		++next;

		if (next == param_types.end()) {
			LOG(it->get_as_text() << ")\n");
			continue;
		}

		LOG(it->get_as_text() << ", ");
	}
	LOG("return type: " << return_datatype.get_as_text() << '\n');
	LOG("num params: " << numArgs << '\n');



	//throw scriptError_t(&*it, "gg");
}

size_t function_c::parse_parameters(code_segment_t& declarations, size_t numArgs)
{
	variable_declaration_t decl(scope, declarations);

	declarations = decl.declare_and_initialize(false); //initializers are not allowed here

	param_types.push_back(std::move(decl.get_type()));

	if (declarations.it == declarations.end) {
		return numArgs;
	}

	++declarations.it;
	
	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
	};

	if (!is_comma(*declarations.it)) {
		throw scriptError_t(&*declarations.it, std::format("expected \",\", found \"{}\"", declarations.it->string));
	}

	++declarations.it; //skip comma

	return parse_parameters(declarations, numArgs + 1);

}

void function_c::parse_returntype(code_segment_t& code)
{
	const auto is_arrow = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_ARROW;
	};

	if (!is_arrow(*code.it)) {
		throw scriptError_t(&*code.it, std::format("expected \"->\", found \"{}\"", code.it->string));
	}

	++code.it; //skip the arrow

	variable_declaration_t decl(scope, code);

	return_datatype = decl.get_variable_declaration_type(code);

	//std::cout << datatype.get_as_text() << '\n';


}
void function_c::create_function(script_t& script, code_segment_t& _code)
{

	if (const auto is_opening_curly_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
		}(*_code.it) == false) {
		throw scriptError_t(&*_code.it, std::string("expected \"{\", but found \"") + _code.it->string + "\"");
	}

	script.S_SetIterator(_code.it);

	set_range_for_scope(script, scope);



}
