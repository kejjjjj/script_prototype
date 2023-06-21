
#ifndef _declaratio
#define _declaratio

#include "pch.hpp"
#include "script.hpp"

class declaration_t {
public:
	declaration_t(const token_statement_t& expression) : tokens(expression)
	{

	};

	bool is_ready() const noexcept { return tokens.it != tokens.end; }

private:

	token_statement_t tokens;

};


#endif