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