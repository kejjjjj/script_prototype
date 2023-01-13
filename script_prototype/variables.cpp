#include "pch.h"


Variable::Variable(const std::string_view& _name, VarType _type, VariableValue init, int32_t _stacklevel) : name(_name), type(_type), stacklevel(_stacklevel)
{


	switch (_type) {
	case VarType::VT_INT:
		value.integer = init.integer;
		break;
	case VarType::VT_FLOAT:
		value.value = init.value;
		break;
	case VarType::VT_STRING:
		value.string = init.string;
		break;
	}

}

void Variable::Initialize(const std::string_view & _name, VarType _type, VariableValue init, int32_t _stacklevel) {

	name = _name;
	value = init;
	stacklevel = _stacklevel;
	type = _type;

	switch (_type) {
	case VarType::VT_INT:
		value.integer = init.integer;
		break;
	case VarType::VT_FLOAT:
		value.value = init.value;
		break;
	case VarType::VT_STRING:
		value.string = init.string;
		break;
	}

}
//uses the same rules as C language
bool VariableNameIsLegal(const std::string_view& var)
{
	if (!std::isalpha(var[0])) {
		CompilerError("expected an identifier");
		return false;
	}

	for (const auto& i : var) {

		if (!std::isalnum(i) && i != '\n' && i != '_' && i != '-') {
			CompilerError("unexpected character '", i, "' in expression");
			return false;
		}

	}
	return true;
}

//assumes the string before the operator is passed
std::string GetVariableTypeString(const std::string expr)
{
	const size_t pos = expr.find_first_of(' ');

	if (pos == std::string::npos)
		return "";

	return expr.substr(0, pos);

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

//assumes the whole string before the operator
bool IsVariableInitialization(const std::string_view& expr)
{

	const std::string variableName = RemoveBlanksFromBeginningAndEnd(expr);

	return GetVariableTypeString(variableName).empty() == false;

}
bool VariableInStack(const std::string_view& var)
{
	for (auto& i : scriptStack.stack) {
		if (!i.name.compare(var)) {
			return true;
		}
	}

	return false;
}
Variable* FindVariableFromStack(const std::string_view& var)
{
	for (auto& i : scriptStack.stack) {
		if (!i.name.compare(var)) {
			return &i;
		}
	}
	return 0;
}