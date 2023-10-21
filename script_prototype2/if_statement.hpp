#ifndef istatment
#define istatment

#include "script.hpp"
#include "scope.hpp"

class if_statement
{
public:
	if_statement(scr_scope_t* scope, const code_segment_t& _statement) : statement(_statement), block(scope) {

	}

	void evaluate_statement();

private:
	scr_scope_t* block = 0;
	code_segment_t statement;
};

#endif