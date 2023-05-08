#include "pch.h"


Variable::Variable(const std::string_view& _name, VarType _type, bool bArray) : name(_name), type(_type), Array(0){
	switch (type) {
		case VarType::VT_INT:
			value.buffer = value.buffer = std::shared_ptr<void*>(new void*);
			value.buf_size = sizeof(int);

			break;
		case VarType::VT_FLOAT:
			value.buffer = value.buffer = std::shared_ptr<void*>(new void*);
			value.buf_size = sizeof(float);
			break;
		case VarType::VT_STRING:
			throw std::exception("rvalue(VarType type): VT_STRING case not supported");
			break;
	}

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

void DeclareVariable(const var_declr_data& data)
{
	std::cout << "pushing \"" << data.variable_name<< "\" of type '" << data.variable_type << "' to stack!\n";

	const auto itype = (VarType)GetDataType(data.variable_type);

	if ((int)itype < 1) {
		throw std::exception("DeclareVariable(): impossible scenario!");
	}

	if (stack_variables.find(std::string(data.variable_name)) != stack_variables.end()) {
		throw std::exception(std::format("the variable \"{}\" is already defined", data.variable_name).c_str());
	}

	stack_variables.insert(std::make_pair(data.variable_name, Variable(data.variable_name, itype)));
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