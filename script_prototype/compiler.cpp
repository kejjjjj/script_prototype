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
	
	//std::cout << std::format("[{}]", RemoveBlanksFromBeginningAndEnd(expr->preOP))

	//std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();


	Expression expr(expression_str);

	expr.ParseExpression(expression_str);

	//std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	//std::chrono::duration<float> difference = now - old;

	//printf("time taken: %.6f\n", difference.count());

	//std::cout << "expression: " << expr.RemoveWhiteSpaces(expression_str) << '\n';

//	expr.RemoveWhiteSpaces(expression_str);

	return true;
}