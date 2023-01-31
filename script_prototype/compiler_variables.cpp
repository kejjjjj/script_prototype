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