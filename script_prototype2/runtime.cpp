#include "runtime.hpp"
#include "scope.hpp"
#include "statement.hpp"
#include "r_expression.hpp"
#include "r_operator_standard.hpp"
#include "r_unary.hpp"
#include "r_postfix.hpp"
#include "r_declaration.hpp"

//assumes that the code has no syntax errors
void runtime_execute(script_t& script)
{
    for (auto& i : compilerInfo) {
        std::cout << "size: " << i.dataSize << " with type: " << static_cast<std::underlying_type_t<decltype(i.type)>>(i.type) << '\n';
    }

    //return;
    runtimeEvaluationFunctions::getInstance().createFunctions();

	script.S_ResetIterator();


    script.global_scope = new scr_scope_t;

    std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();


    try {
        auto it = script.S_GetIterator();
        auto& end = script.S_GetEnd();

        script.global_scope->set_range
        (
            codepos_t{ .line = it->line,  .column = it->column },
            codepos_t{ .line = end->line, .column = end->column }
        );

        auto statement = compilerInfo.begin();

        while (statement != compilerInfo.end()) {

            RuntimeCodeblock_read(script, &script.global_scope, *statement);

            statement++;
        }
    }
    catch (runtimeError_t& err) {
        std::cout << "\n******** SCRIPT RUNTIME ERROR ********\n"
            << err.what() << "\n\n";
    }

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    std::chrono::duration<float> difference = now - old;
    printf("time taken: %.6f\n", difference.count());

    delete script.global_scope;

}

void RuntimeCodeblock_read(script_t& script, scr_scope_t** codeblock, compiler_information& codepos)
{
    std::unique_ptr<r_expression_t> expression;
    std::unique_ptr<r_declaration_t> declaration;

    scr_scope_t* block = *codeblock;




    switch (codepos.type) {
    case compiler_statements_e::EXPRESSION:

        expression = std::unique_ptr<r_expression_t>(new r_expression_t(block, codepos));

        expression->EvaluateEntireExpression();


        break;
    case compiler_statements_e::DECLARATION:

        declaration = std::unique_ptr<r_declaration_t>(new r_declaration_t(block, codepos));

        declaration->declare_and_initialize();
        break;
    }

}