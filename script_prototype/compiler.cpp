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

		if (std::isspace(ch) || !std::isalnum(ch)) {
			auto type = Compile_EvaluateStatement(expression_str);

			if (type != StatementType::NO_STATEMENT) {

				while (std::isspace(ch)) {
					ch = fs::F_Get(f);
				}

				expression_str = (expression_str + FillStatement(ch, f)); //statement + (contents within the parantheses)

				std::cout << "statement: [" << expression_str << "]\n";

				expression_str.clear();
				continue;
			}
		}

		if (ch == ';') {

			CompilerExpression expr(expression_str, this->rtScript);

			expr.ParseExpression(expression_str);
			expressions_parsed++;
			expression_str.clear();
			rtScript += ";";
			continue;
		}

		expression_str.push_back(ch);
	}



	std::cout << "---------------------\n";

	std::cout << rtScript << '\n';

	std::cout << "---------------------\n";

	//runtime script starts here
	expression_str.clear();
	scriptStack.stack.clear();

	//std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();

	//std::cout << "RUNTIME\n";

	//for (const auto& i : rtScript) {

	//	if (i == ';') {

	//		RuntimeExpression expr(expression_str);

	//		expr.ParseExpression(expression_str);
	//		expression_str.clear();
	//		continue;

	//	}
	//	expression_str.push_back(i);

	//}

	//std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	//std::chrono::duration<double> difference = now - old;


	//printf("runtime parsing: %.12f\n", difference.count());

	//runtime script ends here

	//std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();


	//std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	//std::chrono::duration<float> difference = now - old;

	//printf("time taken: %.6f\n", difference.count());

	std::cout << "expressions parsed: " << expressions_parsed << '\n';

	return true;
}