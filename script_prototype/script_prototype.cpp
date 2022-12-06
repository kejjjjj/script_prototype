// script_prototype.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int Com_HashString(const char* string)
{
    char* _string; // esi
    char v2; // al
    unsigned __int8 v3; // bl
    char v4; // di
    char v5; // al

    _string = (char*)string;
    v2 = *_string;
    v3 = 0;
    if (*_string)
    {
        v4 = 119 - *_string;
        do
        {
            v5 = *(v4 + _string++) * tolower(v2);
            v3 += v5;
            v2 = *_string;
        } while (*_string);
    }
    return v3;
}

int main()
{

    operatorHash[HashString("+")] = ADDITIVE;
    operatorHash[HashString("-")] = ADDITIVE;
    operatorHash[HashString("*")] = MULTIPLICATIVE;
    operatorHash[HashString("/")] = MULTIPLICATIVE;
    operatorHash[HashString("%")] = MULTIPLICATIVE;
    operatorHash[HashString("&")] = BITWISE_AND;
    operatorHash[HashString("==")] = EQUALITY;
    operatorHash[HashString("!=")] = EQUALITY;
    operatorHash[HashString("<<")] = SHIFT;
    operatorHash[HashString(">>")] = SHIFT;
    operatorHash[HashString("!")] = UNARY;
    operatorHash[HashString(">=")] = RELATIONAL;
    operatorHash[HashString("<=")] = RELATIONAL;
    operatorHash[HashString(">")] = RELATIONAL;
    operatorHash[HashString("<")] = RELATIONAL;
    operatorHash[HashString("&&")] = LOGICAL_AND;
    operatorHash[HashString("||")] = LOGICAL_OR;
    operatorHash[HashString("|")] = BITWISE_OR;
    operatorHash[HashString("^")] = BITWISE_XOR;
    operatorHash[HashString("~")] = UNARY;



    Script compiler("scripts\\main.kej");

    compiler.Compile();
    
    //uint32_t hash = HashString("hellooooo") % 32;

   // std::cout << "hash: " << hash << '\n';

    //CompilerError("unable to initialize index[", 3, "] for reading");

    system("pause");
}