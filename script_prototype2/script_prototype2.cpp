#include "pch.hpp"
#include "script.hpp"
#include "expression.hpp"

int main()
{
    std::cout << "Hello World!\n";

    script_t script("scripts\\main.kej");
    token_t token;
    
    if (!script.scriptReady()) {
        std::cout << "failed to load the script!\n";
    }
    else {
        try {
            script.S_Tokenize();
            auto expression = script.S_CreateExpression();

            expression_t e(expression);

            if (e.is_ready()) {
                e.EvaluateEntireExpression();
            }
        }
        catch (scriptError_t& err) {
            MessageBox(NULL, err.what(), "Syntax Error!", MB_ICONERROR);
        }
    }

    system("pause");
}
