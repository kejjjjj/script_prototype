
#ifndef initli_t
#define initli_t

#include "pch.hpp"
#include "statement.hpp"
#include "variable.hpp"
#include "scope.hpp"
#include "declaration.hpp"

class initializer_list_t
{
public:
	initializer_list_t(scr_scope_t* scope, const code_segment_t& _statement, Variable* _target);

	void evaluate();
	
	static std::optional<code_segment_t> find_curlybracket_substring(const code_segment_t& token);

private:
	void evaluate_list(Variable* target);
	std::optional<code_segment_t> read_expression(code_segment_t& _statement);

	code_segment_t statement;
	Variable* targetVar = 0;
	scr_scope_t* block = 0;
};
#endif
