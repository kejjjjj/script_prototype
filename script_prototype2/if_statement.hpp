#ifndef istatment
#define istatment

#include "script.hpp"


class if_statement
{
public:
	if_statement(const token_statement_t& _statement) : statement(_statement) {

	}

	void evaluate_statement();

private:

	token_statement_t statement;
};

#endif