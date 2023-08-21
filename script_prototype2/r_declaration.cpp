#include "r_declaration.hpp"
#include "r_expression.hpp"
#include "r_operator_standard.hpp"
#include "runtime.hpp"

r_declaration_t::r_declaration_t(scr_scope_t* scope, const compiler_information& declaration) : block(scope) {

	void* src = declaration.data.get();
	auto ptr = (declaration_t_compiler*)src;


	std::copy(ptr->tokens.begin(), ptr->tokens.end(), tokens.begin());


	//tokens.insert(tokens.begin(), ptr->tokens.begin(), ptr->tokens.end());
	//std::copy(ptr->tokens.begin(), ptr->tokens.end(), tokens.begin());
	std::copy(ptr->typeModifiers.begin(), ptr->typeModifiers.end(), typeModifiers.begin());

	initializer_list = ptr->initializer_list;
	has_initializer = ptr->has_initializer;
	if (initializer_list || has_initializer) {
		std::for_each(ptr->initializer.begin(), ptr->initializer.end(), [this](compiler_information& i) {
			initializers.push_back(std::move(i));
			});
		
		//std::move(ptr->initializer.begin(), ptr->initializer.end(), initializers.begin()); //uhhhh this is not good and has to be fixed
	}
	identifier = ptr->identifier;
	dtype = ptr->dtype;


}

void r_declaration_t::declare_and_initialize()
{
	target = block->declare_variable(var_declr_data({ dtype, identifier, NULL }));

	apply_modifiers(*target);

	std::cout << "declaring a variable of type \"" << target->s_getvariabletype() << "\" with the name of \"" << target->identifier << "\"\n";

	if (has_initializer) {
		initialize();

	}
}
void r_declaration_t::apply_modifiers(Variable& _target)
{
	if (typeModifiers.empty()) {
		return;
	}

	auto modifier = typeModifiers.front();
	typeModifiers.pop_front();

	switch (modifier) {
	case declaration_modifiers_e::ARRAY:
		_target.create_array(); //create an array of size 1
		return apply_modifiers(_target.arrayElements[0]); //and then do this to every child
	}


}

void r_declaration_t::initialize()
{
	if (initializer_list) {
		throw runtimeError_t(&tokens.front(), "runtime initializer lists not supported");
		return;
	}

	r_expression_t expression(block, initializers.front());
	auto result = expression.EvaluateEntireExpression();

	static auto f = runtimeEvaluationFunctions::getInstance().find_function(P_ASSIGN);

	r_expression_token t;
	t.lval = target;
	t.op = false;
	t.set_token(result.get_token());
	
	f(t, result);

}