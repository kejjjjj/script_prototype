#include "functions.hpp"
#include "expression.hpp"
#include "declaration.hpp"
#include "parsing_utils.hpp"

size_t parse_parameters(scr_scope_t*& scope, code_segment_t& declarations, size_t numArgs, std::unique_ptr<function_scope>& function);
void parse_returntype(scr_scope_t*& scope, code_segment_t& _code, std::unique_ptr<function_scope>& function);
void create_function(script_t& script, scr_scope_t*& scope, code_segment_t& _code);


void emit_function_declaration(script_t& script, scr_scope_t*& scope, code_segment_t code)
{
	if (scope->is_global_scope() == false)
		throw scriptError_t(&*code.it, "a function declaration is only allowed in the global scope");


	std::unique_ptr<function_scope> function = std::move(std::unique_ptr<function_scope>(new function_scope));

	scope = create_scope_without_range(scope);

	auto& it = code.it;
	auto end = code.end;

	++it; //skip the fn keyword

	if (it->tt != tokenType::NAME) {
		throw scriptError_t(&*it, std::format("expected an identifier, found \"{}\"", it->string));
	}

	function->identifier = it->string;

	++it; //skip identifier

	if (it->tt != tokenType::PUNCTUATION || (LOWORD(it->extrainfo) != P_PAR_OPEN)) {
		throw scriptError_t(&*it, std::format("expected a \"(\""));
	}

	if (auto args_opt = find_matching_parenthesis(code_segment_t{ .it = code.it, .begin = code.it, .end = (++code.end--) })) {
		auto& args = args_opt.value();

		args.print();

		function->numArgs = parse_parameters(scope, args, 1, function);

		LOG("function with " << function->numArgs << " parameters!\n");

		it = ++args.end;
	}
	else {
		LOG("no args!\n");
		++it;
	}

	++it; //skip )
	code.end = end;
	parse_returntype(scope, code, function);

	create_function(script, scope, code);

	LOG("\n--- function info ---\n");
	LOG("name: " << function->identifier << '\n');
	LOG("params: (");
	for (auto iter = function->param_types.begin(); iter != function->param_types.end(); ++iter) {
		
		auto next = iter;
		++next;

		if (next == function->param_types.end()) {
			LOG(iter->get_as_text() << ")\n");
			continue;
		}

		LOG(iter->get_as_text() << ", ");
	}
	LOG("return type: " << function->return_datatype.get_as_text() << '\n');
	LOG("num params: " << function->numArgs << '\n');

	scope->add_to_function_table(*function.get());
	scope->make_function_scope(function);

	std::cout << "function -> " << scope->is_function_scope() << '\n';

	//throw scriptError_t(&*it, "gg");
}

size_t parse_parameters(scr_scope_t*& scope, code_segment_t& declarations, size_t numArgs, std::unique_ptr<function_scope>& function)
{
	variable_declaration_t decl(scope, declarations);

	declarations = decl.declare_and_initialize(false); //initializers are not allowed here

	function->param_types.push_back(std::move(decl.get_type()));

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

	return parse_parameters(scope, declarations, numArgs + 1, function);

}

void parse_returntype(scr_scope_t*& scope, code_segment_t& code, std::unique_ptr<function_scope>& function)
{
	const auto is_arrow = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_ARROW;
	};

	if (!is_arrow(*code.it)) {
		throw scriptError_t(&*code.it, std::format("expected \"->\", found \"{}\"", code.it->string));
	}

	++code.it; //skip the arrow

	variable_declaration_t decl(scope, code);

	function->return_datatype = decl.get_variable_declaration_type(code);

	//std::cout << datatype.get_as_text() << '\n';


}
void create_function(script_t& script, scr_scope_t*& scope, code_segment_t& _code)
{

	if (const auto is_opening_curly_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
		}(*_code.it) == false) {
		throw scriptError_t(&*_code.it, std::string("expected \"{\", but found \"") + _code.it->string + "\"");
	}

	script.S_SetIterator(_code.it);

	set_range_for_scope(script, scope);

	scope->emit_to_lower_scope(statementType_e::FUNCTION_DECLARATION);


}
