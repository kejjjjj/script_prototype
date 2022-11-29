#include "pch.h"


Compiler::Compiler(std::string path)
{
	
	path = fs::GetRootDirectory() + "\\" + path;
	if (!fs::F_OpenFile(f, path, fs::fileopen::FILE_IN)) {
		return;
	}
	std::cout << std::format("\"{}\" successfully opened!\n", path);
	//fs::F_CloseFile(f);

}
Compiler::~Compiler() 
{
	fs::F_CloseFile(f);
}
bool Compiler::Compile()
{
	std::string expression_str;

	fs::F_Reset();

	while (f.good() && !f.eof()) {

		char ch = fs::F_Get(f);

		if (ch == ';')
			break;

		expression_str.push_back(ch);
	}
	
	Expression expr(expression_str);
	std::cout << "expression: " << expr.RemoveWhiteSpaces(expression_str) << '\n';

//	expr.RemoveWhiteSpaces(expression_str);

	return true;
}