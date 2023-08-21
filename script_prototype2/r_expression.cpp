#include "r_expression.hpp"
#include "r_operator_standard.hpp"
#include "runtime.hpp"
r_expression_token r_expression_t::EvaluateEntireExpression()
{
	std::cout << "EvaluateEntireExpression(";
	for (auto& it : sortedTokens) {
		std::cout << it.get_token().string << ' ';
	}
	std::cout << ")\n";

	EvaluateExpression();

	return std::move(result);
}

void r_expression_t::EvaluateExpression()
{
	std::for_each(sortedTokens.begin(), sortedTokens.end(), [this](r_expression_token& t) {
		t.eval_postfix(block);
		t.eval_prefix();
	});

	EvaluateExpressionTokens();
	
}
void r_expression_t::EvaluateExpressionTokens()
{


	std::list<r_expression_token>::iterator itr1, itr2 = sortedTokens.begin();
	const auto& op_end = --sortedTokens.end();
	OperatorPriority op{}, next_op{};



	while (sortedTokens.size() > 2) {
		itr1 = ++sortedTokens.begin();
		itr2 = itr1;
		std::advance(itr2, 2);

		if (itr2 != sortedTokens.end()) {
			do {

				op = static_cast<OperatorPriority>(itr1->op_priority);
				next_op = static_cast<OperatorPriority>(itr2->op_priority);

				if (itr2 == sortedTokens.end() || itr2 == op_end || next_op <= op)
					break;

				std::advance(itr1, 2);
				std::advance(itr2, 2);

			} while (next_op > op);

		}
		itr2 = itr1;


		auto& Operator = itr1->get_token();
		auto& leftVal = --itr1;
		auto& rightVal = ++itr2;

		using FunctionType = std::function<r_expression_token(r_expression_token&, r_expression_token&)>;

		const auto& evalFunctions = runtimeEvaluationFunctions::getInstance();
		const auto punctuationType = static_cast<punctuation_e>(LOWORD(Operator.extrainfo));
		const FunctionType function = evalFunctions.find_function(punctuationType);

		*itr2 = function(*leftVal, *rightVal);

		sortedTokens.erase(itr1, itr2);
	}

	result = std::move(*itr2);
}