#include "pch.h"


//remove all whitespaces
//keep all quoted strings
std::string Expression::RemoveWhiteSpaces(std::string& expr)
{

	//parsing is easier when there are no unnecessary characters
	std::string converted_str, temp;
	size_t tokens = GetStringTokens(expr, '"');

	//this would never be valid syntax unless it is \"
	if (tokens % 2 != 0)  CompilerError(std::format("Odd number of quotation marks in string:\n {}", expr));
	
	//no quotation marks so remove all whitespaces
	if (tokens == NULL) converted_str = RemoveBlank(expr);

	//string has quotation marks so remove everything outside of them
	else {
		std::vector<std::string> strTok;
		TokenizeString(expr, '"', strTok);
		std::cout << '\n';
		int j = 0;
		for (const auto& i : strTok) {



			if (j % 2 == 0 /*|| (j == strTok.size() - 1u)*/) { //not within quotations
				temp.clear();
				RemoveBlank(strTok[j], temp);

				
				converted_str.resize(converted_str.size() + temp.size());
				converted_str += temp;
				j++;

			}
			else {
				
				converted_str.resize(converted_str.size() + strTok[j].size() + 2);
				converted_str += '"';
				converted_str += strTok[j];
				converted_str += '"';
				j++;
			}
			//std::cout << std::format("[{}]: {}\n", j++, i);
			


		}

	}

	return converted_str;
}
void Expression::TokenizeExpression()
{

}

bool Expression::EvaluateExpression()
{
	return true;
}