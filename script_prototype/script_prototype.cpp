// script_prototype.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
int main()
{
    int a = 2;
    int b = a + a++ + a++ + a++;

    std::cout << "b: " << b << '\n';

    Script compiler("scripts\\main.kej");

    compiler.Compile();
    

    system("pause");
}