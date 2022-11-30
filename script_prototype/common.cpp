#include "pch.h"

std::string RemoveFromString(std::string& str, char a)
{
    std::string string;

    for (const auto& i : str) {
        if (i == ' ')
            continue;

        string.push_back(i);
    }

    return string;
}
SIZE_T GetStringTokens(std::string& expr, char delim)
{
    SIZE_T tokens(0);

    for (const auto& i : expr)
        if (i == delim)
            ++tokens;

    return tokens; //apparently this can never be 0?
}
SIZE_T TokenizeString(std::string& expr, char delim, std::vector<std::string>& tokens)
{
    std::stringstream ss(expr);

    std::string token;



    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }

    return tokens.size();
}
std::string RemoveBlank(std::string& expr)
{
    std::string fixed;
    for (const auto& i : expr)
        if (!std::isblank(i))
            fixed.push_back(i);

    return fixed;
}
size_t RemoveBlank(std::string& expr, std::string& out)
{
    for (const auto& i : expr)
        if (!std::isblank(i))
            out.push_back(i);

    return out.size();
}
std::string RemoveBlanksFromBeginningAndEnd(std::string& in)
{
    std::string out;

    bool parseBeginning = true;
    bool parseEnd = false;
    int32_t index(-1);
    for (auto& i : in) {
        index++;
        if (parseBeginning && std::isblank(i)) {
            
            continue;
        }

        parseBeginning = false;

        

        if (std::isblank(i)) {

            //test if the rest of the characters are also blank
            int chars_left = in.size() - index;
            int blanks_found(0);
            for (int j = index; j < in.size(); j++) {
                if (std::isblank(in[j]))
                    ++blanks_found;
            }
            if (chars_left == blanks_found)
                return out;

        }
        
        out.push_back(i);

    }
    return out;
}
void CompilerError(std::string str, ...)
{
    str += std::format("\n\nLine: {}\nColumn: {}", fs::file.lines_read, fs::file.current_column);

    const char* msg = str.c_str();
    char v2[4096];



    va_list va;

    va_start(va, msg);
    _vsnprintf_s(v2, 0x1000u, msg, va);
    v2[4095] = 0;

    MessageBoxA(NULL, v2, "Compiler Error!", MB_ICONERROR);
    exit(-1);

}