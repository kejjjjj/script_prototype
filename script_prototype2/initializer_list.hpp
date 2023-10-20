
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
	initializer_list_t(scr_scope_t* scope, const token_statement_t& _statement, Variable* _target);

	void evaluate();
	
	static std::optional<token_statement_t> find_curlybracket_substring(const token_statement_t& token);

	friend class variable_declaration_t_compiler;
private:
	void evaluate_list(Variable* target);
	std::optional<token_statement_t> read_expression(token_statement_t& _statement);

	token_statement_t statement;
	Variable* targetVar = 0;
	scr_scope_t* block = 0;
};
#endif
