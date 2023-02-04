#include "pch.h"


bool cv::ParseDeclarationType(const std::string_view& full_statement)
{
	const std::string cut = RemoveBlanksFromBeginning(full_statement);
	std::string varType = GetVariableTypeString(cut);

	const size_t type = GetDataType(varType);

	if (!type) {
		return false;
	}

	return true;

}
bool cv::ParseTypeQualifier(const std::string_view& full_statement)
{
	const std::string cut = RemoveBlanksFromBeginning(full_statement);
	std::string varType = GetVariableTypeString(cut);
	
	const size_t type = GetTypeQualifier(varType);

	if (!type) {
		return false;
	}

	return true;
}
bool cv::ParseIdentifier(const std::string_view& full_statement)
{
	const std::string cut = RemoveBlanksFromBeginning(full_statement);

	if (!std::isalpha(cut.front())) {
		return false;
	}

	return true;

}
bool cv::ParseInitializer(const std::string_view& full_statement)
{
	const std::string cut = RemoveBlanksFromBeginning(full_statement);

	if (cut != "=")
		return false;

	
	return true;

}
bool cv::ParseNumericValue(const std::string_view& full_statement)
{
	return ValidNumber(full_statement);
}
std::string cv::ParseOperator(std::string::iterator& it)
{
	std::string token;
	
	//parse the operator
	if (!IsOperator(*it)) {
		return "";
	}

	token.push_back(*it); //push the first operator

	//insert to the operator as long as it is a valid operator
	while (true) {
		++it;
		token.push_back(*it);


		if (!IsAnOperator(token)) {
			token.pop_back(); //remove the last operator since it is invalid
			--it;
			break;
		}

	}


	

	return token;
}