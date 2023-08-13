#include "scope.hpp"
#include "statement.hpp"
#include "expression.hpp"
#include "declaration.hpp"
#include "if_statement.hpp"
#include "initializer_list.hpp"

void Codeblock_read(script_t& script, scr_scope_t* block)
{

    std::unique_ptr<expression_t> expression;
    std::unique_ptr<declaration_t> declaration;


    auto statement = script.S_CreateStatement();
    auto statement_type = statement_determine(statement);

    switch (statement_type) {
    case statementType_e::EXPRESSION:
        expression = std::unique_ptr<expression_t>(new expression_t(block, statement));

        if ((expression->is_ready()))
            expression->EvaluateEntireExpression();

        break;
    case statementType_e::DECLARATION:
        declaration = std::unique_ptr<declaration_t>(new declaration_t(block, statement));

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

        if (block->is_global_scope()) {
            throw scriptError_t(&*statement.it, "found \"}\" but it's not closing anything");
        }

        block = block->on_exit();

        break;
    case statementType_e::SCOPE:

        script.S_SetIterator(statement.begin);

        //FIX MEEEEEEEE
        scr_scope_t scope(block);
        std::make_shared<scr_scope_t>(block);
        *block = std::move(scope);

        std::optional<token_statement_t> bracket;
        auto remaining = script.S_GiveRemaining();

        remaining.it = statement.it;

        if ((bracket = initializer_list_t::find_curlybracket_substring(remaining)) == std::nullopt) {
            throw scriptError_t("HOW ON EARTH!!!!");
        }

        script.S_SetIterator(++statement.begin);

        auto& end = bracket.value();

        block->set_range
        (
            codepos_t{ .line = statement.it->line,  .column = statement.it->column },
            codepos_t{ .line = end.end->line,        .column = end.end->column }
        );

       // throw scriptError_t(&*statement.it, "gg");

        


        break;

    }



}
