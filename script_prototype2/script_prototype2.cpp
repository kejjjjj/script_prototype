#include "pch.hpp"
#include "script.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"
#include "o_standard.hpp"
#include "o_unary.hpp"
#include "o_postfix.hpp"
#include "if_statement.hpp"
#include "scope.hpp"
#include "runtime.hpp"

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

    script.global_scope = new scr_scope_t;
    
    try {
        script.S_Tokenize();

        auto it = script.S_GetIterator();
        auto& end = script.S_GetEnd();

        script.global_scope->set_range
        (
            codepos_t{ .line = it->line,  .column = it->column },
            codepos_t{ .line = end->line, .column = end->column }
        );


        while(!script.is_eof()){

            Codeblock_read(script, &script.global_scope);
          
          

        }

        if (!script.global_scope->is_global_scope()) {
            throw scriptError_t(&*script.S_GetIterator(), "expected to find a \"}\"");
        }

    }
    catch (scriptError_t& err) {
        std::cout << "\n******** SCRIPT COMPILE ERROR ********\n"
            << err.what() << "\n\n";
        //MessageBox(NULL, err.what(), "Syntax Error!", MB_ICONERROR);
    }

    std::cout << "\n------- global variables -------\n\n";
    script.global_scope->print_localvars();
    std::cout << "\n--------------------------------\n";

    delete script.global_scope;

    std::cout << "\n******** BEGIN RUNTIME ********\n";


    runtime_execute(script);

    system("pause");
}
