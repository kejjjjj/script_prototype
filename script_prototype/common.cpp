#include "pch.h"

uint32_t HashString(const char* s)
{
    uint32_t hash = 0;

    for (; *s; ++s)
    {
        hash += *s;
        hash += (hash << 10);
        hash ^= (hash >> 7);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % 513;
}
uint32_t HashString(const std::string_view& s_)
{
    const std::string _s = std::string(s_.begin(), s_.end());
    const char* s = _s.c_str();

    //std::cout << "s: " << s << '\n';

    uint32_t hash = 0;

    for (; *s; ++s)
    {
        hash += *s;
        hash += (hash << 10);
        hash ^= (hash >> 7);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % 513;
}
float Eval(const float& a, const float& b, const std::string_view& ops)
{
    if (ops.size() < 2) {
        char op = ops[0];
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
        case '>':
            return a > b;
        case '<':
            return a < b;
        case '&':
            return (int)a & (int)b;
        case '|':
            return (int)a | (int)b;
        case '^':
            return (int)a ^ (int)b;
        case '%':

            if ((int)a != a || (int)b != b) {
                CompilerError("Modulo % operator can only be used on integer types");
                return false;
            }
            else if (b == 0) {
                CompilerError("Division by zero");
                return false;
            }
            return (int)a % (int)b;
            

        }
    }

    //has 2 ops
    if (ops == "==") 
        return a == b;
    
    else if (ops == "!=")
        return a != b;

    else if (ops == ">=")
        return a >= b;

    else if (ops == "<=")
        return a <= b;

    else if (ops == "||")
        return a || b;

    else if (ops == "&&")
        return a && b;
    
    else if (ops == "<<") {
        if (b < 0) {
            CompilerError("Shift count is negative: ", a, " << ", b);
            return false;
        }
        return (int)a << (int)b;
    }
    else if (ops == ">>") {
        if (b < 0) {
            CompilerError("Shift count is negative: ", a, " >> ", b);
            return false;
        }
        return (int)a >> (int)b;
    }

    CompilerError("Unknown operator: ", ops);

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
SIZE_T TokenizeString(std::string& expr, char delim, std::vector<std::string>& tokens)
{
    std::stringstream ss(expr);

    std::string token;



    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }

    return tokens.size();
}
SIZE_T TokenizeStringOperands(const std::string_view& expr, std::list<std::string>& tokens)
{
    std::string token;

    std::string a = RemoveBlank(expr);
    int32_t idx = -1;
    char previous_character{'\0'};
    size_t subtr_in_a_row{0}, equals_in_a_row{0}, size = a.size();
    auto begin = a.begin();
    auto end = a.end();
    for (auto it = begin; it != end; ++it) {

        auto& i = *it;
        idx++;

        //this block is for ==
        if (i == '=' && *(it - 1) != '!') { //!= is false

            //std::cout << "yep: " << i << *(it -1) << '\n';
            if (!equals_in_a_row) {
                //save token before the operator (if it's not empty)
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }

            equals_in_a_row++;

            // == is true here
            if (equals_in_a_row == 2) {

                token.push_back('=');
                token.push_back('=');
                tokens.push_back(token);
                token.clear();

            }
            //invalid syntax
            else if (equals_in_a_row > 2) {
                CompilerError("unrecognized operator '==='");
                return false;
            }
            continue;
        }
        else
            equals_in_a_row = 0;

        if (IsCalculationOp(i)) {
            if(i == '-')
                subtr_in_a_row++;

            if (subtr_in_a_row > 2) { //works for cases like / -(-1) A.K.A 3 consecutive operators

                if (subtr_in_a_row % 1 == 0) {//odd number 
                    //make the number positive
                    token.pop_back();
                  //  std::cout << "making it positive\n";
                }
                else {
                    //make the number negative
                    token.push_back(i);
                  //  std::cout << "making it negative\n";
                }
                continue;
            }

            //a check for negative numbers
            if (idx == 0 || i == '-' && previous_character != '\0') {
                token.push_back(i);
                previous_character = i;
               // std::cout << "negative value detected after an operator.. skipping!\n";
                continue;
            }
        

            //save the string before the operator
            if (token.empty()) {
                CompilerError("Expected an expression");
                return false;
            }

            tokens.push_back(token);
            token.clear();

            //push first operator
            token.push_back(i);

            if (i == '&' || i == '|' || i == '<' || i == '>') {
                ++it;

                i = *it;

                if (i == '&' || i == '|' || i == '<' || i == '>') {
                    token.push_back(i);
                   // std::cout << "duplicate " << i << '\n';
                }
                else
                    --it;

                equals_in_a_row = 0;
            }

            
            //make sure buffer overflow won't happen
            if (idx < size - 2) {
                //is the character after the = an another =
                if (*(it + 1) == '=') { // ==
                    token.push_back('=');
                    
                }
                if (i == '!' && *(it + 1) == '=') {
                   // token.push_back('='); //!=
                    it++;
                }

            }

            //save the operator
            tokens.push_back(token);
            token.clear();

            previous_character = i;
            continue;
        }
            
        //assignments are forbidden
        //returns false if previous character is not !, < or > (!=, <=, >=)
        char previous = a[idx - 2];
        if (equals_in_a_row == 1 && previous != '!' && previous != '<' && previous != '>') { //one = and ! doesn't exist (!=)
            CompilerError("Assignments are not supported yet!: ", a[idx - 2], '=');
            return false;
        }

        token.push_back(i);
        previous_character = '\0';
        subtr_in_a_row = 0;
        equals_in_a_row = 0;
    }
    if (token.empty()) {
        CompilerError("Expected an expression");
        return false;
    }
    tokens.push_back(token);

    return tokens.size();
}
SIZE_T GetCharacterCount(const std::string_view& str, char c)
{
    SIZE_T count=0;
    for (auto& i : str) {
        if (i == c)
            count++;
    }
    return count;
}
//does not include the parantheses
Parenthesis_s GetStringWithinParentheses(const std::string_view& expr)
{
    int32_t idx = -1;
    int32_t opening{0}, closing{0}, count_opening{0}, count_closing{0};
    for (const auto& i : expr) {
        idx++;

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

    const int len = closing - opening - count_opening;
    //note: cuts out the parantheses
    return { count_opening, count_closing, opening, len, expr.substr(opening + 1, len) };
}
std::string RemoveBlank(const std::string_view& expr)
{
    std::string fixed;
    for (const auto& i : expr)
        if (!std::isspace(i))
            fixed.push_back(i);

    return fixed;
}
size_t RemoveBlank(std::string_view& expr, std::string& out)
{
    for (const auto& i : expr)
        if (!std::isspace(i))
            out.push_back(i);

    return out.size();
}
std::string RemoveIrrelevantCode(const std::string_view& expr)
{
    bool within_quotes = false;
    std::string fixed;
    for (const auto& i : expr)
    {
        if (i == '"') {
            within_quotes = !within_quotes;
        }if (within_quotes) {
            fixed.push_back(i);
            continue;
        }

        if (std::isspace(i))
            continue;


        fixed.push_back(i);

    }

    if (within_quotes) {
        CompilerError("missing closing quote");
        return "";
    }
    return fixed;
}
std::string RemoveBlanksFromBeginningAndEnd(const std::string_view& in)
{
    std::string out;

    bool parseBeginning = true;
    bool parseEnd = false;
    int32_t index(-1);
    for (auto& i : in) {
        index++;
        if (parseBeginning && std::isspace(i)) {
            
            continue;
        }

        parseBeginning = false;

        

        if (std::isspace(i)) {

            //test if the rest of the characters are also blank
            int chars_left = in.size() - index;
            int blanks_found(0);
            for (int j = index; j < in.size(); j++) {
                if (std::isspace(in[j]))
                    ++blanks_found;
            }
            if (chars_left == blanks_found)
                return out;

        }
        
        out.push_back(i);

    }
    return out;
}
std::string RemoveDuplicateBlanks(const std::string_view& in)
{
    size_t in_a_row = 0;
    std::string fixed;

    for (const auto& i : in) {

        if (std::isspace(i)) {
            in_a_row++;
        }
        else
            in_a_row = NULL;

        if (in_a_row >= 2) {
            continue;
        }

        fixed.push_back(i);

    }
    return fixed;
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


OperatorPriority GetOperandPriority(const std::string_view& ops)
{

   // return operatorHash[HashString(ops)];

    if (ops.size() == 1) {
        char op = ops[0];


        if (op == '|') // Bitwise OR	
            return BITWISE_OR;

        if (op == '^') //Bitwise XOR	
            return BITWISE_XOR;

        else if (op == '&') //Bitwise AND	
            return BITWISE_AND;

        else if (op == '>' || op == '<') //Relational
            return RELATIONAL;

        else if (op == '+' || op == '-')
            return ADDITIVE;

        else if (op == '*' || op == '/' || op == '%')
            return MULTIPLICATIVE;


        CompilerError("Unknown operator '", ops, "'");
        return FAILURE;
    }
    
    //two operators

    if (ops == "==" || ops == "!=")
        return EQUALITY;

    else if (ops == "<=" || ops == ">=")
        return RELATIONAL;

    else if (ops == "<<" || ops == ">>")
        return SHIFT;

    else if (ops == "&&")
        return LOGICAL_AND;

    else if (ops == "||")
        return LOGICAL_OR;


    CompilerError("Unknown operator [", ops, "]");
    return FAILURE;

}
bool ValidNumber(const std::string_view& expr)
{
    size_t dot_count{0}, dot_idx, index{ 0 }, sub_count{0}, sub_idx;

    if (expr.size() == 1) {
        if (!std::isalnum(expr[0]) && expr[0] != '_')
            return false;
    }

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
//CALL AFTER ISVALIDNUMBER
bool IsInteger(const std::string_view& expr)
{
    return expr.find('.') == std::string_view::npos; //no decimal
}
void PushString(std::string* strptr, const std::string_view& str)
{
    *strptr += str;
}
VarType StringType(const std::string_view& expr)
{
    if (!ValidNumber(expr))
        return VarType::VT_STRING;

    if (IsInteger(expr))
        return VarType::VT_INT;

    return VarType::VT_FLOAT;
}
std::string to_string(const float val, const bool integer)
{
    if (integer)
        return std::to_string((int64_t)val);

    return std::to_string(val);
}