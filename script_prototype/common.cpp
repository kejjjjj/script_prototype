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
SIZE_T TokenizeStringOperands2(const std::string_view& expr, std::list<std::string>& tokens)
{
    auto begin = expr.begin();
    auto end = expr.end();
    CHAR i{};
    std::string token;
    bool ParseValue = true;

    constexpr int32_t MAX_OPERATORS_IN_ROW = 3;

    for (auto it = begin; it != end; it++) {

        i = *it;

        
        if (ParseValue) {

            while (IsPrefixOp(*it)) { //push all the prefixes
                token.push_back(*it);
                ++it;
            }



            if (!IsValidSyntaxForName(*it) && *it != '.' && *it != '"') {
                //if (*it == '.' && std::isdigit(*(it - 1))) {
                //    goto fine; //decimal point
                //}

                CompilerError("the character '", *it, "' was unexpected");
                return 0;
            }
            while (IsValidSyntaxForName(*it) || *it == '.' || *it == '"') {
                token.push_back(*it);
                ++it;
            }
            tokens.push_back(token);
            token.clear(); //clear the token because it is saved
            --it;
            ParseValue = false;
            continue;
        }

        //parse the operator
        if (!IsOperator(i)) {
            CompilerError("TokenizeStringOperands2(): expected an expression: [", *(it-1), i, "]");
            return 0;
        }

        token.push_back(i); //push the first operator

        //insert to the operator as long as it is a valid operator
        while (true) {
            ++it;
            token.push_back(*it);

            
            if (!IsAnOperator(token)) {
                token.pop_back(); //remove the last operator since it is invalid
                break;
            }

        }



        tokens.push_back(token); //save the operator
        token.clear(); //clear the token because it is saved

        --it;

        ParseValue = true;

    }

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
std::string RemoveBlanksFromBeginning(const std::string_view& in)
{
    std::string out;
    size_t idx = 0;

    while (std::isspace(in[idx])) 
    {
        idx++;
    }

    out = in.substr(idx);
    return out;
}
size_t FindFirstBlank(const std::string_view& in)
{
    std::string out;
    size_t idx = 0;

    for (auto& i : in) {

        if (std::isspace(i))
            return idx;

        idx++;
    }

    return std::string::npos;

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

        if (op == '=') // Assignment	
            return ASSIGNMENT;

        if (op == '|') // Bitwise OR	
            return BITWISE_OR;

        else if (op == '^') //Bitwise XOR	
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

    else if (ops == "+=" || ops == "-=" || ops == "*=" || ops == "/=" || ops == "%=" || ops == ">>=" || ops == "<<=" || ops == "&=" || ops == "^=" || ops == "|=")
        return ASSIGNMENT;

   // CompilerError("Unknown operator [", ops, "]");
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
std::string HasPrefix(const std::string_view& str)
{
    std::string prefix;
    auto beg = str.begin();
    const auto begin = *beg;

    for (auto& i = beg; i != str.end(); i++) {
        if (IsPrefixOp(*i)) {
            prefix.push_back(*i);
            continue;
        }
        break;
    }

    return prefix;
}
std::string EvalPrefixes(const std::string& value, const std::string_view& prefix)
{

    const auto EvalSingularPrefix = [](char it, std::string& ref) {
       // std::cout << "EvalSingularPrefix(): " << it << " with " << ref << '\n';

        switch (it) {

        case '-':
            if (ref.front() == it) {
                ref.erase(0, 1); //if the - already exists, then remove it
                break;
            }ref.insert(ref.begin(), it); //if the - doesn't exist, then add it

            break;

        case '!':

            ref = Eval(ref, "0", "=="); //I think this is how the ! prefix works (val == false)

            break;
        case '~':

            ref = Eval(ref, "0", "~");

            break;
        default:
            RuntimeError("EvalPrefixes(): impossible [", it , "]");
            return "";
        }
    };

    auto begin = prefix.begin();
    auto end = prefix.end();
    std::string val = value;

    if (prefix.empty())
        return value;
    else if (prefix.size() == 1) {
        EvalSingularPrefix(prefix.front(), val);
        return val;
    }




    for (auto it = end; it != begin; ) { //evaluate from right to left
        std::advance(it, -1);

        EvalSingularPrefix(*it, val);

        

    }

    return val;
}
//this function ONLY expects numbers
//variables and strings are not supported
std::string Eval(const std::string& a, const std::string& b, const std::string_view& ops)
{

    const auto EvalStrings = [&a, &b, &ops]() -> std::string
    {
        const std::string ac = RemoveQuotes(a), bc = RemoveQuotes(b);

        if (ops.size() < 2) {

            switch (ops[0]) {
            case '+':

                return "\"" + ac + bc + "\"";
            default:
                RuntimeError("Illegal operator used on a string expression");
                return "";
            }
        }

        if (ops == "==")
            return (ac == bc) == true ? "1" : "0";

        else if (ops == "!=")
            return (ac != bc) == true ? "1" : "0";

        else if (ops == "||")
            return (!ac.empty() || !bc.empty()) == true ? "1" : "0";

        else if (ops == "&&")
            return (!ac.empty() && !bc.empty()) == true ? "1" : "0";


        RuntimeError("Illegal operator used on a string expression");
        return "";

    };

    const VarType at = StringType(a), bt = StringType(a);

    if (at == VarType::VT_STRING && bt == at) //both are strings
        return EvalStrings();

    else if (at == VarType::VT_STRING && bt != at || bt == VarType::VT_STRING && at != bt) { //a string and a non-string
        RuntimeError("Cannot cast from '", VarTypes[(int)at], "' to '", VarTypes[(int)bt], "'");
        return "";
    }

    float va, vb;


    const auto resa = std::from_chars(a.data(), a.data() + a.size(), va);
    const auto resb = std::from_chars(b.data(), b.data() + b.size(), vb);

    const bool a_int = IsInteger(a);
    const bool b_int = IsInteger(b);

    if (ops == "~" && a_int) {
        return to_string(~(int64_t)va, a_int);
    }

    if (ops.size() < 2) {
        char op = ops[0];
        switch (op) {

        case '+':

            return to_string(
                (a_int == true ? (int64_t)va : va) + (b_int == true ? (int64_t)vb : vb), a_int);
        case '-':
            return to_string(
                (a_int == true ? (int64_t)va : va) - (b_int == true ? (int64_t)vb : vb), a_int);
        case '*':
            return to_string(
                (a_int == true ? (int64_t)va : va) * (b_int == true ? (int64_t)vb : vb), a_int);
        case '/':

            if (vb == 0) {
                RuntimeError("Division by zero");
                return "0";

            }

            return to_string(
                (a_int == true ? (int64_t)va : va) / (b_int == true ? (int64_t)vb : vb), a_int);

        case '>':
            return to_string(
                (a_int == true ? (int64_t)va : va) > (b_int == true ? (int64_t)vb : vb), a_int);
        case '<':
            return to_string(
                (a_int == true ? (int64_t)va : va) < (b_int == true ? (int64_t)vb : vb), a_int);
        case '&':
            if (!a_int || !b_int) {
                RuntimeError("'", op, "' operator used with non-integral operands");
                return "";
            }

            return to_string(
                (int64_t)va & (int64_t)vb, a_int);
        case '|':
            if (!a_int || !b_int) {
                RuntimeError("'", op, "' operator used with non-integral operands");
                return "";
            }

            return to_string(
                (int64_t)va | (int64_t)vb, a_int);
        case '^':
            if (!a_int || !b_int) {
                RuntimeError("'", op, "' operator used with non-integral operands");
                return "";
            }

            return to_string(
                (int64_t)va ^ (int64_t)vb, a_int);

        case '%':
            if (!a_int || !b_int) {
                RuntimeError("'", op, "' operator used with non-integral operands");
                return "";
            }
            else if (vb == 0) {
                RuntimeError("Division by zero");
                return "";
            }
            return to_string(
                (int64_t)va % (int64_t)vb, a_int);
        }
    }

    if (ops == "==") {
        return  ((a_int == true ? (int64_t)va : va) == (b_int == true ? (int64_t)vb : vb)) == true ? "1" : "0";
    }
    else if (ops == "!=") {
        return  ((a_int == true ? (int64_t)va : va) != (b_int == true ? (int64_t)vb : vb)) == true ? "1" : "0";
    }
    else if (ops == ">=") {
        return  ((a_int == true ? (int64_t)va : va) >= (b_int == true ? (int64_t)vb : vb)) == true ? "1" : "0";
    }
    else if (ops == "<=") {
        return  ((a_int == true ? (int64_t)va : va) <= (b_int == true ? (int64_t)vb : vb)) == true ? "1" : "0";
    }
    else if (ops == "||") {
        return  ((a_int == true ? (int64_t)va : va) || (b_int == true ? (int64_t)vb : vb)) == true ? "1" : "0";
    }
    else if (ops == "&&") {
        return  ((a_int == true ? (int64_t)va : va) && (b_int == true ? (int64_t)vb : vb)) == true ? "1" : "0";
    }
    else if (ops == "<<") {

        if (!a_int || !b_int) {
            RuntimeError("'", ops, "' operator used with non-integral operands");
            return "";
        }

        else if (vb < 0) {
            RuntimeError("shift count is negative");
            return "";
        }

        return  to_string((int64_t)va << (int64_t)vb, a_int);
    }
    else if (ops == ">>") {

        if (!a_int || !b_int) {
            RuntimeError("'", ops, "' operator used with non-integral operands");
            return "";
        }

        else if (vb < 0) {
            RuntimeError("shift count is negative");
            return "";
        }

        return  to_string((int64_t)va >> (int64_t)vb, a_int);
    }


    RuntimeError("Unknown operator: ", ops);

    return "";

}
bool IsAnOperator(const std::string_view& op)
{
    

    if (op.size() == 1) {
        return IsOperator(op.front());
    }

    return (op == "==" || op == "!=" || op == ">=" || op == "<=" || op == ">>" || op == "<<" || op == "&&" || op == "||");

}
bool IsAnOperator2(const std::string_view& op)
{
    
    return IsAnOperator(op) || (op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == ">>=" || op == "<<=" || op == "&=" || op == "^=" || op == "%=" || op == "|=");
}
bool EndOfOperator(const std::string_view& op)
{
    return (op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == ">>=" || op == "<<=" || op == "&=" || op == "^=" || op == "%=" || op == "|=" || op == "++" || op == "--" || op == "!=");
}
bool NextOperatorCanBeAnOperator(const std::string_view& op)
{

    if (op.back() == '=')
        return false;

    const auto size = op.size();

    if (size > 2)
        return false;
    
    if ((op.front() == '~')) //unary
        return false;


    return true;

}
bool CompatibleDataTypes(const VarType a, const VarType b)
{
    return !(a == VarType::VT_STRING && b != a || b == VarType::VT_STRING && a != b);
}
std::string RemoveQuotes(const std::string& str)
{
    if (str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }

    return str;
}
std::string ExtractStatementName(const std::string_view& expr)
{
    std::string out;
    size_t idx = 0;

    out = RemoveBlanksFromBeginning(expr);

    for (auto& i : expr) {

        if (std::isspace(i) || !std::isalnum(i))
            break;

        idx++;
    }

    return std::string(expr.substr(0, idx));
}