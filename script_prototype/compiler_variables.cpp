#include "pch.h"


bool cv::ParseVariableDeclaration(const std::string_view& full_statement)
{
	std::string cut = RemoveBlanksFromBeginning(full_statement);
	std::string varType = GetVariableTypeString(cut);

	const size_t type = GetDataType(varType);

	if (!type) {
		return false;
	}

	return true;

}