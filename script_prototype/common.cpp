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
SIZE_T TokenizeString(const std::string& expr, char delim, std::list<std::string>& tokens)
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
Substr_s GetStringWithinCharacters(const std::string_view& expr, const char s, const char e)
{
    int32_t idx = -1;
    int32_t opening{0}, closing{0}, count_opening{0}, count_closing{0};
    for (const auto& i : expr) {
        idx++;

        if (i == s) {
            count_opening = 1;
            opening = idx;
        }
        else if (i == e) {
            count_closing = 1;
            closing = idx;
        }

        if (count_opening > 0 && count_opening == count_closing) {
            break;
        }

        
        
    }

    const int len = closing - opening - count_opening;
    //note: cuts out the s and e characters
    std::string result_string = std::string(expr).substr(opening + 1, len);
    if (count_opening && count_closing && result_string.empty())
        result_string = ("empty");

    return { count_opening, count_closing, opening, len, result_string };
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
    size_t dot_count{0}, dot_idx, index{ 0 }, sub_count{0};

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

        if (dot_idx == expr.back()) //last character can be a dot
            return true;

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
bool IsAnOperator(const std::string_view& op)
{
    

    if (op.size() == 1) {
        return IsAnyOperator(op.front());
    }

    return (op == "==" || op == "!=" || op == ">=" || op == "<=" || op == ">>" || op == "<<" || op == "&&" || op == "||");

}
bool SatisfiesOperator(const std::string_view& op)
{
    if (op.size() == 1) {
        return IsOperator(op.front());
    }

    return IsAnOperator2(op);

    //return (op == "==" || op == "!=" || op == ">=" || op == "<=" || op == ">>" || op == "<<" || op == "&&" || op == "||");

}
bool IsAnOperator2(const std::string_view& op)
{
    return IsAnOperator(op) || (op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == ">>=" || op == "<<=" || op == "&=" || op == "^=" || op == "%=" || op == "|=" || op == "<=>" || op == "++" || op == "--");
}
bool EndOfOperator(const std::string_view& op)
{
    return (op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == ">>=" || op == "<<=" || op == "&=" || op == "^=" || op == "%=" || op == "|=" || op == "++" || op == "--" || op == "!=");
}
bool IsUnaryOperator(const std::string_view& op)
{
    if (op.size() == 1) {
        auto o = op.front();
        return (o == '+' || o == '-' || o == '!' || o == '~' || o == '?');
    }
    return UnaryArithmeticOp(op); //++ or --
        
}
bool IsPostfixOperator(const std::string_view& op)
{
    return UnaryArithmeticOp(op) || op == "["; //++ or --
}