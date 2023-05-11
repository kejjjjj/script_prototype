#include "pch.h"


Variable::Variable(const std::string_view& _name, VarType _type) : name(_name), type(_type){
	AllocateValues();
}
Variable::~Variable()
{
	//delete value.buffer;
	//value.buf_size = 0;

	//lol try to remember to delete it :p
}

bool IsDataType(const std::string_view& str)
{
	for (const auto& i : VarTypes)
		if (!str.compare(i))
			return true;

	return false;
}
VarType GetDataType(const std::string_view& str)
{
	for (size_t i = 0; i < VarTypes.size(); i++){
		if (!str.compare(VarTypes[i]))
			return (VarType)i;
	}

	return VarType::VT_INVALID;
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

Variable* DeclareVariable(const std::string& name, const VarType type)
{
	std::cout << "pushing \"" << name<< "\" of type '" << VarTypes[int(type)] << "' to stack!\n";

	if (type == VarType::VT_FLOAT) {
		throw std::exception("DeclareVariable(): impossible scenario!");
	}

	if (stack_variables.find(std::string(name)) != stack_variables.end()) {
		throw std::exception(std::format("the variable \"{}\" is already defined", name).c_str());
	}

	return &stack_variables.insert(std::make_pair(name, Variable(name, type))).first->second;
}

declr_type DeclarationUnaryToType(char op)
{
	std::cout << "testing " << op << '\n';

	if (op == '?') {
		throw std::exception("references are not supported yet");
		return declr_type::REFERENCE;
	}
	if (op == '[')
		return declr_type::ARRAY;

}