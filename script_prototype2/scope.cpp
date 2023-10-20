#include "scope.hpp"
#include "statement.hpp"
#include "expression.hpp"
#include "declaration.hpp"
#include "if_statement.hpp"
#include "initializer_list.hpp"

void Codeblock_read(script_t& script, std::unique_ptr<scr_scope_t>& codeblock)
{

    std::unique_ptr<expression_t> expression;
    std::unique_ptr<variable_declaration_t> declaration;


    scr_scope_t* block = codeblock.get();

    auto statement_opt = script.S_CreateStatement(block);

    if (!statement_opt) {
        //no value means no statement
        return;
    }

    auto& statement = statement_opt.value();

    auto statement_type = statement_determine(statement);


    switch (statement_type) {
    case statementType_e::EXPRESSION:
        expression = std::unique_ptr<expression_t>(new expression_t(block, statement));

        if ((expression->is_ready()))
            expression->EvaluateEntireExpression();

        break;
    case statementType_e::DECLARATION:
        declaration = std::unique_ptr<variable_declaration_t>(new variable_declaration_t(block, statement));

        if ((declaration->is_ready()))
            declaration->declare_and_initialize();

        break;
    case statementType_e::STATEMENT_KEYWORD:

        switch (static_cast<statementKeywords_e>(statement.it->extrainfo)) {

        case statementKeywords_e::IF:
            if_statement(block, statement).evaluate_statement();
            break;

        default:
            throw scriptError_t(&*statement.it, "statementKeywords_e: default case");

        }

        break;
    case statementType_e::SCOPE_EXIT:

        break;
    case statementType_e::SCOPE:
        


        break;

    }



}

scr_scope_t* create_scope(script_t& script, const scr_scope_t* block)
{
    LOG("creating a new scope\n");
    scr_scope_t* scope = (new scr_scope_t);

    scope->set_lower_scope(block);

    std::optional<token_statement_t> bracket;
    auto remaining = script.S_GiveRemaining();

    remaining.it = script.S_GetIterator();

    if ((bracket = initializer_list_t::find_curlybracket_substring(remaining)) == std::nullopt) {
        throw scriptError_t("HOW ON EARTH!!!!");
    }

    script.S_SetIterator(++remaining.it);

    auto& end = bracket.value();

    scope->set_range
    (
        codepos_t{ .line = remaining.it->line,  .column = remaining.it->column },
        codepos_t{ .line = end.end->line,        .column = end.end->column }
    );

    return scope;
}
scr_scope_t* delete_scope(script_t& script, scr_scope_t* codeblock)
{
    scr_scope_t* temp_block = 0;

    if (codeblock->is_global_scope()) {
        throw scriptError_t(&*script.S_GetIterator(), "found \"}\" but it's not closing anything");
    }

    if (!script.is_eof())
        script.S_SetIterator(++script.S_GetIterator());

    temp_block = codeblock->on_exit(); //get the lower scope ptr
    delete codeblock;

    return temp_block;
}