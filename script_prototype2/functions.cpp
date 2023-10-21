#include "functions.hpp"
#include "expression.hpp"


void function_c::parse_declaration()
{
	if (scope->is_global_scope() == false)
		throw scriptError_t(&*code.it, "a function declaration is only allowed in the global scope");

	auto& it = code.it;

	++it; //skip the fn keyword

	if (it->tt != tokenType::NAME) {
		throw scriptError_t(&*it, std::format("expected an identifier, found \"{}\"", it->string));
	}

	identifier = it->string;

	++it; //skip identifier

	if (it->tt != tokenType::PUNCTUATION || (LOWORD(it->extrainfo) != P_PAR_OPEN)) {
		throw scriptError_t(&*it, std::format("expected a \"(\""));
	}

	auto full_segment = code_segment_t{ .it = code.it, .begin = code.it, .end = (++code.end--) };
	expression_t::ExpressionFindMatchingParenthesis(full_segment);
	const code_segment_t args = code_segment_t{ .it = ++it, .begin = it, .end = --full_segment.it };

	args.print();

	throw scriptError_t(&*it, "gg");
}