
#ifndef initli_t
#define initli_t

#include "pch.hpp"
#include "statement.hpp"
#include "variable.hpp"

class initializer_list_t
{
public:
	initializer_list_t(const token_statement_t& _statement, Variable& _target);

	void evaluate();

private:
	void evaluate_list(Variable& target);
	std::optional<token_statement_t> find_curlybracket_substring(const token_statement_t& token);
	void tokenize_lists(std::list<token_statement_t>& statements);
	void initialize_array(Variable& targetArray);
	void tokenize_values(std::list<token_statement_t>& statements);
	token_statement_t statement;
	Variable& targetVar;
};

#endif