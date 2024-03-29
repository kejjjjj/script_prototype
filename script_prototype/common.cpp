#include "pch.h"

//does not include the parantheses
Substr_s GetStringWithinCharacters(const std::string_view& expr, const char s, const char e)
{
    size_t idx = 0;
    size_t opening{0}, closing{0}, count_opening{0}, count_closing{0};
    for (const auto& i : expr) {
        

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

        idx++;
        
    }

    const size_t len = closing - opening - count_opening;
    //note: cuts out the s and e characters
    std::string result_string = std::string(expr).substr(opening + 1, len);
    if (count_opening && count_closing && result_string.empty())
        result_string = ("empty");

    return { count_opening, count_closing, opening, len, result_string };
}
OperatorPriority GetOperatorPriority(const std::string_view& ops)
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
}
bool IsAnOperator2(const std::string_view& op)
{
    return IsAnOperator(op) || (op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == ">>=" || op == "<<=" || op == "&=" || op == "^=" || op == "%=" || op == "|=" || op == "<=>" || op == "++" || op == "--");
}
bool IsUnaryOperator(const std::string_view& op)
{
    if (op.size() == 1) {
        auto o = op.front();
        return (o == '+' || o == '-' || o == '!' || o == '~' || o == '@');
    }
    return UnaryArithmeticOp(op); //++ or --
        
}
bool IsPostfixOperator(const std::string_view& op)
{
    return UnaryArithmeticOp(op) || op == "["; //++ or --
}


bool StringIsBlank(const std::string_view& s)
{
    for (const auto& i : s)
        if (!std::isspace(i))
            return false;

    return true;
}