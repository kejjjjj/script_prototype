#include "pch.h"

float Eval(const float& a, const float& b, char op)
{
    switch (op) {

    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        if (b == 0) {
            CompilerError("Division by zero");
            return 0;
        }
        return a / b;

    }
    return 0;
}

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
SIZE_T GetStringOperandTokens(std::string_view& expr)
{
    SIZE_T tokens(0);

    for (const auto& i : expr)
        if (i == '+' || i == '-' || i == '*' || i == '/')
            ++tokens;

    return tokens;
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
SIZE_T TokenizeStringOperands(const std::string_view& expr, std::vector<std::string>& tokens)
{
    
    std::string token;

    std::string a = RemoveBlank(expr);
    int32_t idx = -1;
    char previous_character{'\0'};
    size_t subtr_in_a_row{0};
    for (const auto& i : a) {
        idx++;
        if (i == '+' || i == '-' || i == '*' || i == '/') {
            subtr_in_a_row++;

            if (subtr_in_a_row > 2) { //works for cases like / -(-1) A.K.A 3 consecutive operators

                if (subtr_in_a_row % 1 == 0) {//odd number 
                    token.pop_back();
                  //  std::cout << "making it positive\n";
                }
                else {
                    token.push_back(i);
                  //  std::cout << "making it negative\n";
                }
                continue;
            }

            if (idx == 0 || i == '-' && previous_character != '\0') {
                token.push_back(i);
                previous_character = i;
               // std::cout << "negative value detected after an operand.. skipping!\n";
                continue;
            }
            
            tokens.push_back(token);
            token.clear();
            token.push_back(i);
            tokens.push_back(token);
            token.clear();
            previous_character = i;
            continue;
        }

        token.push_back(i);
        previous_character = '\0';
        subtr_in_a_row = 0;
    }
    tokens.push_back(token);

    return tokens.size();
}
Parenthesis_s GetStringWithinParenthesis(const std::string_view& expr)
{
    int32_t idx = -1;
    int32_t opening{0}, closing{0}, count_opening{0}, count_closing{0};
    for (const auto& i : expr) {
        idx++;

      //  std::cout << "i[" << idx << "] = " << i << '\n';
        switch (i) {

            case '(':
                count_opening = 1;
                opening = idx;
                break;
            case ')':
                count_closing = 1;
                closing = idx;
                break;

            default:
                break;
        }
        if (count_opening > 0 && count_opening == count_closing) {
            break;
        }

        
        
    }

  //  std::cout << std::format("( count = {}\n) count = {}\n( = {}\n) = {}\n", count_opening, count_closing, opening, closing);
    const int len = closing - opening - count_opening;
    return { count_opening, count_closing, opening, len, expr.substr(opening + 1, len) };
}
std::string RemoveBlank(const std::string_view& expr)
{
    std::string fixed;
    for (const auto& i : expr)
        if (!std::isblank(i))
            fixed.push_back(i);

    return fixed;
}
size_t RemoveBlank(std::string_view& expr, std::string& out)
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
//void CompilerError(std::string str, ...)
//{
//    str += std::format("\n\nLine: {}\nColumn: {}", fs::file.lines_read, fs::file.current_column);
//
//    const char* msg = str.c_str();
//    char v2[4096];
//
//
//
//    va_list va;
//
//    va_start(va, msg);
//    _vsnprintf_s(v2, 0x1000u, msg, va);
//    v2[4095] = 0;
//
//    MessageBoxA(NULL, v2, "Compiler Error!", MB_ICONERROR);
//    exit(-1);
//
//}


OperatorPriority GetOperandPriority(char op)
{

    //return ((op == '+' || op == '-') == true) ? LOW : MEDIUM;

    if (op == '+' || op == '-')
        return LOW;

    else if (op == '*' || op == '/')
        return MEDIUM;


    return LOW;

}
bool ValidNumber(const std::string_view& expr)
{
    size_t dot_count{0}, dot_idx, index{ 0 }, sub_count{0}, sub_idx;

    for (const auto& i : expr) {
        switch (i) {

        case '.': //has a decimal
            dot_count++;

            if (dot_count > 1)
                return false;
            dot_idx = index;
            break;
        case '-': //is a negative number

            if (index != 0)  //only the first character can be this
                return false;

            break;

        default: 

            if (!std::isdigit(i))
                return false;
            break;
        }
        index++;

    }

    if (dot_count) {

        if (dot_idx == expr.size() - 1) //last character cannot be a dot
            return false;

        if (!std::isdigit(expr[dot_idx + 1])) //next character is not a number
            return false;
    }


    return true;
}