#include "pch.hpp"
#include "script.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"
#include "o_standard.hpp"
#include "o_unary.hpp"
#include "o_postfix.hpp"
#include "if_statement.hpp"

int main()
{
    std::cout << "Hello World!\n";

    script_t script("scripts\\main.kej");
    token_t token;
    
    if (!script.scriptReady()) {
        std::cout << "failed to load the script!\n";
        return 0;
    }

    evaluationFunctions::getInstance().createFunctions();
    unaryFunctions::getInstance().createFunctions();
    postfixFunctions::getInstance().createFunctions();

    std::unique_ptr<expression_t> expression;
    std::unique_ptr<declaration_t> declaration;

    

    try {
        script.S_Tokenize();
        while(!script.is_eof()){

          
            auto statement = script.S_CreateStatement();
            auto statement_type = statement_determine(statement);

            switch (statement_type) {
            case statementType_e::EXPRESSION:
                expression = std::unique_ptr<expression_t>(new expression_t(statement));

                if ((expression->is_ready()))
                    expression->EvaluateEntireExpression();

                break;
            case statementType_e::DECLARATION:
                declaration = std::unique_ptr<declaration_t>(new declaration_t(statement));

                if ((declaration->is_ready()))
                    declaration->declare_and_initialize();

                break;
            case statementType_e::STATEMENT_KEYWORD:

                switch (static_cast<statementKeywords_e>(statement.it->extrainfo)) {

                case statementKeywords_e::IF:
                    if_statement(statement).evaluate_statement();
                    break;

                default:
                    throw scriptError_t(&*statement.it, "statementKeywords_e: default case");

                }

                break;
            }

        }
    }
    catch (scriptError_t& err) {
        std::cout << "\n******** SCRIPT COMPILE ERROR ********\n"
            << err.what() << "\n\n";
        //MessageBox(NULL, err.what(), "Syntax Error!", MB_ICONERROR);
    }


    VariableTable::getInstance().print();

    system("pause");
}
