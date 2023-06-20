#ifndef expr_tok
#define expr_tok

#include "pch.hpp"
#include "script.hpp"
#include "rvalue.hpp"

struct expression_token
{
	expression_token() = default;
	expression_token(token_t& _token) : token(_token) {

	}
	void insert_prefix(token_t& token) { prefix.push_back(&token); }
	void insert_postfix(token_t& token) { postfix.push_back(&token); }
	void set_token(token_t& _token) { token = _token; }
	token_t& get_token() noexcept { return token; }

	void set_value_category();

	void print() const noexcept
	{
		std::cout << "\n---------TOKEN---------\n";
		for (const auto& i : prefix)
			std::cout << i->string << ' ';
		std::cout << '\n';
		std::cout << token.string << '\n';
		for (const auto& i : postfix)
			std::cout << i->string << ' ';

		std::cout << '\n';



	}

	bool op = false;
	std::shared_ptr<rvalue> rval;
private:
	
	std::list<token_t*> prefix;
	std::list<token_t*> postfix;
	token_t token;
};


#endif