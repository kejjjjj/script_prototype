// script_prototype.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main()
{
    Compiler compiler("scripts\\main.kej");

    compiler.Compile();
    
    //CompilerError("unable to initialize index[", 3, "] for reading");

    system("pause");
}