
#include "expression_token.hpp"

void expression_token::set_value_category()
{
	if (op)
		return;

	//passing &token is fine because the rvalue's lifetime is the same as this expression_token :clueless:

	switch (token.tt) {
	case tokenType::NUMBER_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({
			.type=dataTypes_e::INT, 
			.token=&token}));
		rval->set_value<int32_t>(std::stoi(token.string));
		return;
	case tokenType::FLOAT_LITERAL:
		rval = std::shared_ptr<rvalue>(new rvalue({ 
			.type = dataTypes_e::FLOAT, 
			.token = &token }));
		rval->set_value<float>(std::stof(token.string));
		return;
	default:
		throw scriptError_t(&token, "this value type is not supported yet :)");

	}

}