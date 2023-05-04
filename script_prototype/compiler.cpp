#include "pch.h"


Script::Script(const std::string& _path)
{
	std::string path = _path;
	path = fs::GetRootDirectory() + "\\" + path;
	std::cout << "path: " << path << '\n';
	if (!fs::F_OpenFile(f, path, fs::fileopen::FILE_IN)) {
		return;
	}
	std::cout << std::format("\"{}\" successfully opened!\n", path);

	f_str = fs::F_FileStreamToString(f);

	if (f_str.empty()) {
		fs::F_CloseFile(f);
		CompilerError("unable to read file contents");
		return;
	}

	fs::F_CloseFile(f);

	//fs::F_CloseFile(f);

}
//Script::~Script()
//{
//	//std::cout << "Script::~Script()\n";
//	//fs::F_CloseFile(f);
//}
bool Script::Compile()
{
	std::cout << "compiling project!\n";

	std::chrono::time_point<std::chrono::system_clock> old = std::chrono::system_clock::now();

	auto begin = f_str.begin();
	try {

		while (begin != f_str.end()) {

			code_type code = cec::Compiler_ReadNextCode3(begin);
			std::cout << "code: " << code.code << '\n';

			if (code.type != code.DECLARATION)
				expr::EvaluateEntireExpression(code.code.substr(0, code.code.size()));

			srules.reset();
			syntax.ClearFlags();
			expr::rules.reset();
			++begin;
		}
	}
	catch (std::exception& ex) {
		std::cout << "exception caught: " << ex.what() << '\n';

	}

	//std::cout << "\ncode: " << code.code << '\n';

	

	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	std::chrono::duration<float> difference = now - old;
	printf("time taken: %.6f\n", difference.count());

	std::cout << "\nscript stack: \n";

	for (auto& i : stack_variables) {
		std::cout << i.second.name << '<' << VarTypes[int(i.second.get_type())] << "> (";

		switch (i.second.get_type()) {
		case VarType::VT_INT:
			std::cout << i.second.get_int() << ")\n";
			break;
		case VarType::VT_FLOAT:
			std::cout << i.second.get_float() << ")\n";
			break;
		case VarType::VT_STRING:
			std::cout << i.second.get_string() << ")\n";
			break;
		}

	}

	return true;
}