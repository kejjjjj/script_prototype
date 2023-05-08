// script_prototype.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
int main()
{

    Script compiler("scripts\\main.kej");

    compiler.Compile();
    

    system("pause");
}