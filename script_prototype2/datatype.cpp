#include "datatype.hpp"
#include "script.hpp"
dataTypes_e typeFromIntegerSuffix(const DWORD typeInfo)
{
	literalSuffixType suffix = static_cast<literalSuffixType>(typeInfo);

	switch (suffix) {
	case literalSuffixType::NONE:
			return dataTypes_e::INT;

	case literalSuffixType::CHAR:
		return dataTypes_e::CHAR;
	default:
		throw scriptError_t("wtf unsupported suffix");
		return dataTypes_e::UNKNOWN;
	}


}

std::string get_type_as_text(const dataTypes_e type) 
{

	auto& data = dataTypeTable::getInstance().get_list();

	auto begin = data.begin();

	std::advance(begin, static_cast<ptrdiff_t>(type));

	if (begin == data.end())
		return "NULL";

	return *begin;
	
}