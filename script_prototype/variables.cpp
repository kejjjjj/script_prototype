#include "pch.h"


Variable::Variable(const std::string_view& _name, VarType _type) : name(_name), type(_type)
{

}

bool IsDataType(const std::string_view& str)
{
	for (const auto& i : VarTypes)
		if (!str.compare(i))
			return true;

	return false;
}
size_t GetDataType(const std::string_view& str)
{
	for (size_t i = 0; i < VarTypes.size(); i++){
		if (!str.compare(VarTypes[i]))
			return i;
	}

	return 0u;
}
size_t GetTypeQualifier(const std::string_view& str)
{
	for (size_t i = 0; i < VarQualifiers.size(); i++) {
		if (!str.compare(VarQualifiers[i]))
			return i;
	}

	return 0u;
}
bool IsConst(const std::string_view& v)
{
	if (ValidNumber(v))
		return true;
	
	return false;
}
void DeclareVariable(const std::string_view& type, const std::string_view& name)
{
	std::cout << "pushing \"" << name << "\" of type '" << type << "' to stack!\n";

	auto itype = (VarType)GetDataType(type);

	if ((int)itype < 1) {
		throw std::exception("DeclareVariable(): impossible scenario!");
	}

	if (stack_variables.find(name) != stack_variables.end()) {
		throw std::exception(std::format("the variable \"{}\" is already defined", name).c_str());
	}

	Variable var(name, itype);
	stack_variables.insert(std::make_pair(name, var));
}

