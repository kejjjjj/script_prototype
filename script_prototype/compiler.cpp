#include "pch.h"


Script::Script(const std::string& _path)
{
	std::string path = _path;
	path = fs::GetRootDirectory() + "\\" + path;
	if (!fs::F_OpenFile(f, path, fs::fileopen::FILE_IN)) {
		return;
	}
	std::cout << std::format("\"{}\" successfully opened!\n", path);
	//fs::F_CloseFile(f);

}
Script::~Script()
{
	fs::F_CloseFile(f);
}
bool Script::Compile()
{
	std::cout << "compiling project!\n";
	std::string expression_str;

	fs::F_Reset();

	size_t expressions_parsed = 0;
	while (f.good() && !f.eof()) {

		char ch = fs::F_Get(f);

		if (ch == ';') {

			CompilerExpression expr(expression_str, this->rtScript);

			expr.ParseExpression(expression_str);
			expressions_parsed++;
			expression_str.clear();
			rtScript += ";\n";
			continue;
		}

		expression_str.push_back(ch);
	}
	
	std::cout << "---------------------\n";

	std::cout << rtScript << '\n';

	std::cout << "---------------------\n";

	//std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();


	//std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	//std::chrono::duration<float> difference = now - old;

	//printf("time taken: %.6f\n", difference.count());

	std::cout << "expressions parsed: " << expressions_parsed << '\n';

	return true;
}