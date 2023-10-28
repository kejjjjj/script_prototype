#include "scope.hpp"
#include "statement.hpp"
#include "expression.hpp"
#include "declaration.hpp"
#include "if_statement.hpp"
#include "initializer_list.hpp"
#include "functions.hpp"
#include "return_statement.hpp"
void Codeblock_read(script_t& script, scr_scope_t** codeblock)
{

    std::unique_ptr<variable_declaration_t> declaration;


    scr_scope_t*& block = *codeblock;
    auto statement_opt = script.create_code_segment(&block);
    if (!statement_opt) {
        //no value means no statement
        return;
    }

    auto& statement = statement_opt.value();

    auto statement_type = statement_determine(statement);


    switch (statement_type) {
    case statementType_e::EXPRESSION:
        eval_expression(block, statement);
        break;
    case statementType_e::DECLARATION:
        declaration = std::unique_ptr<variable_declaration_t>(new variable_declaration_t(block, statement));

        if ((declaration->is_ready()))
            declaration->declare_and_initialize(true);

        break;
    case statementType_e::IF_STATEMENT:
        eval_if_statement(script, block, statement);
        break;
    case statementType_e::ELSE_STATEMENT:
        eval_else_statement(script, block, statement);
        break;
    case statementType_e::FUNCTION_DECLARATION:
        emit_function_declaration(script, block, statement);
        break;
    case statementType_e::RETURN:
        eval_return_statement(statement, block);
        break;

    }

}

scr_scope_t* create_scope(script_t& script, scr_scope_t* block)
{
    scr_scope_t* scope = create_scope_without_range(block);
    set_range_for_scope(script, scope);

    return scope;
}
scr_scope_t* create_scope_without_range(scr_scope_t* block)
{
    scr_scope_t* scope = (new scr_scope_t); //one could say this is bad but it's so well memory managed that it's ok :)
    //it gets freed if the script comes across a }
    //or if an exception occurs

    scope->set_lower_scope(block);
    scope->set_function_table(block->get_function_table());


    return scope;
}
void set_range_for_scope(script_t& script, scr_scope_t* scope)
{
    std::optional<code_segment_t> bracket;
    auto remaining = script.S_GiveRemaining();

    remaining.it = script.S_GetIterator();

    if ((bracket = initializer_list_t::find_curlybracket_substring(remaining)) == std::nullopt) {
        throw scriptError_t("expected a \"{\"");
    }


    auto& end = bracket.value();

    scope->set_range
    (
        codepos_t{ .line = remaining.it->line,  .column = remaining.it->column },
        codepos_t{ .line = end.end->line,        .column = end.end->column }
    );

    script.S_SetIterator(++remaining.it);

}
scr_scope_t* delete_scope(script_t& script, scr_scope_t* codeblock)
{
    LOG("deleting the scope\n");

    scr_scope_t* temp_block = 0;

    if (codeblock->is_global_scope()) {
        throw scriptError_t(&*script.S_GetIterator(), "found \"}\" but it's not closing anything");
    }

    if (function_scope* function_scope = codeblock->is_function_scope()) {

        if (codeblock->get_lower()->is_global_scope()) {
            if (function_scope->function_will_return == false) {
                throw scriptError_t(&*script.S_GetIterator(), "the function must return a value at the lowest scope");
            }
            LOG("the function \"" << function_scope->identifier << "\" returns a value\n");

        }

    }

    if (!script.is_eof())
        script.S_SetIterator(++script.S_GetIterator());

    temp_block = codeblock->on_exit(); //get the lower scope ptr
    delete codeblock;

    return temp_block;
}