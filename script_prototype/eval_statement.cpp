#include "pch.h"

//returns statement type
StatementType Compile_EvaluateStatement(const std::string_view& expr)
{
	//remove the first blanks
	std::string str = RemoveBlanksFromBeginning(expr);

	auto eval = Compile_EvaluateStatement2(ExtractStatementName(str));

	//if (eval == StatementType::NO_STATEMENT) {
	//	//CompilerError("Expected a statement");
	//	return StatementType::NO_STATEMENT;
	//}

	

	return eval;

}
StatementType Compile_EvaluateStatement2(const std::string_view& expr)
{
	for (auto& [name, statement] : statement_type) {
		if (!expr.compare(name))
			return statement;
	}

	return StatementType::NO_STATEMENT;
}

//called if the last word read was a statement
//fills the characters until the last matching )
std::string FillStatement(char start, std::fstream& f)
{
	if (start != '(') {
		CompilerError("Expected a '('");
		return "";
	}

	std::string out {'('}; // add the opening paranthesis

	size_t opening = 1, closing = 0;

	while (f.good() && !f.eof()) {

		char ch = fs::F_Get(f);

		switch (ch) {
		case '(':
			opening++;
			break;
		case ')':
			closing++;
			break;
		default:
			break;
		}

		out.push_back(ch);

		if (opening == closing)
			break;

	}

	if (opening != closing) {
		CompilerError("Expected a ')'");
		return "";
	}

	return out;



}

void ParseStatement(const StatementType stype, const std::string_view& full_statement)
{
	switch (stype) {

	case StatementType::WHEN_STATEMENT:
		cws::ParseWhenStatement(full_statement);

		break;
	default:
		CompilerError("Unknown statement");
		break;
	}
}