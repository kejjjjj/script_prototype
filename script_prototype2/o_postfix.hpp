
#ifndef o_postfx
#define o_postfx

#include "pch.hpp"
#include "expression_token.hpp"
#include "scope.hpp"

class postfixFunctions
{
public:
	static auto& getInstance() {
		static postfixFunctions instance;
		return instance;
	}

	void createFunctions();

	auto find_function(const punctuation_e p) const {
		const auto found = postfix_functions.find(p);
		return found != postfix_functions.end() ? std::make_optional(found->second) : std::nullopt;
	}

private:
	std::unordered_map<punctuation_e, std::function<void(scr_scope_t*,expression_token&, postfixBase* code)>> postfix_functions;

	static void subscript(scr_scope_t* block, expression_token&, postfixBase* code);
	static void function_call(scr_scope_t* block, expression_token&, postfixBase* code);

};

#endif