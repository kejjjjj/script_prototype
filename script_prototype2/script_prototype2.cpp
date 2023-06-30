#include "pch.hpp"
#include "script.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"
#include "o_standard.hpp"
#include "o_unary.hpp"
#include "o_postfix.hpp"
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

    try {
        script.S_Tokenize();
        while(!script.is_eof()){

          
            auto statement = script.S_CreateStatement();
            auto statement_type = statement_determine(statement);

            if (statement_type == statementType_e::EXPRESSION) {
                expression_t e(statement);

                if (e.is_ready()) {
                    e.EvaluateEntireExpression();
                }
                
            }

            else if (statement_type == statementType_e::DECLARATION) {
                declaration_t e(statement);

                if (e.is_ready()) {
                    e.declare_and_initialize();
                }

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
