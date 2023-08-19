#pragma once

#include "scope.hpp"
#include "r_expression_token.hpp"
#include "expression_token.hpp"

class r_expression_t {
public:
	r_expression_t(scr_scope_t* scope, compiler_information& expression) : block(scope)
	{
		//const size_t numElems = expression.dataSize / sizeof(expression_token_compiler);

		

		for (size_t i = 0; i < expression.dataSize; i += sizeof(expression_token_compiler)) {
			r_expression_token token;
			void* src = expression.data.get() + i;
			auto ptr = (expression_token_compiler*)src;

			//std::cout << "ptr->token.string = " << ptr->token.string << '\n';

			token.get_token() = ptr->token;
			for (auto& a : ptr->prefix) token.prefix.push_back(a);
			for (auto& a : ptr->postfix) token.postfix.push_back(a);

			if (ptr->b_rvalue)
				token.rval = std::unique_ptr<rvalue>(new rvalue(std::move(ptr->rval)));

			token.op = ptr->op;
			token.op_priority = ptr->op_priority;

			sortedTokens.push_back(std::move(token));

		}


	};

	r_expression_token EvaluateEntireExpression();


private:
	void EvaluateExpression();
	void EvaluateExpressionTokens();

	r_expression_token result;
	std::list<r_expression_token> sortedTokens;
	scr_scope_t* block = 0;
};