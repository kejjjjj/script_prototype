#include "pch.hpp"
#include "script.hpp"

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
            script.S_CreateExpression();
        }
        catch (scriptError_t& err) {
            MessageBox(NULL, err.what(), "Syntax Error!", MB_ICONERROR);
        }
    }

    system("pause");
}
