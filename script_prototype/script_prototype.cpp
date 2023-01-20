// script_prototype.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
int main()
{

    //operatorHash[HashString("+")] = ADDITIVE;
    //operatorHash[HashString("-")] = ADDITIVE;
    //operatorHash[HashString("*")] = MULTIPLICATIVE;
    //operatorHash[HashString("/")] = MULTIPLICATIVE;
    //operatorHash[HashString("%")] = MULTIPLICATIVE;
    //operatorHash[HashString("&")] = BITWISE_AND;
    //operatorHash[HashString("==")] = EQUALITY;
    //operatorHash[HashString("!=")] = EQUALITY;
    //operatorHash[HashString("<<")] = SHIFT;
    //operatorHash[HashString(">>")] = SHIFT;
    //operatorHash[HashString("!")] = UNARY;
    //operatorHash[HashString(">=")] = RELATIONAL;
    //operatorHash[HashString("<=")] = RELATIONAL;
    //operatorHash[HashString(">")] = RELATIONAL;
    //operatorHash[HashString("<")] = RELATIONAL;
    //operatorHash[HashString("&&")] = LOGICAL_AND;
    //operatorHash[HashString("||")] = LOGICAL_OR;
    //operatorHash[HashString("|")] = BITWISE_OR;
    //operatorHash[HashString("^")] = BITWISE_XOR;
    //operatorHash[HashString("~")] = UNARY;



    Script compiler("scripts\\main.kej");

   compiler.Compile();
    
    //RuntimeExpression e("a");

    //std::string a = "-233", b = "6", op = "&";

    //constexpr int aaa = -233 & 6;

    //std::cout << a << ' ' << op << ' ' << b << " = " << e.Eval(a, b, op) << '\n';

    //uint32_t hash = HashString("hellooooo") % 32;

   // std::cout << "hash: " << hash << '\n';

    //CompilerError("unable to initialize index[", 3, "] for reading");

    system("pause");
}