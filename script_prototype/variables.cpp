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

void Variable::AllocateValues() {
	switch (type) {
	case VarType::VT_INT:
		value.buffer = value.buffer = std::shared_ptr<void*>(new void*);
		value.buf_size = sizeof(int);
		*reinterpret_cast<int*>(value.buffer.get()) = 0;
		break;
	case VarType::VT_FLOAT:
		value.buffer = value.buffer = std::shared_ptr<void*>(new void*);
		value.buf_size = sizeof(float);
		*reinterpret_cast<float*>(value.buffer.get()) = 0;

		break;
	case VarType::VT_STRING:
		throw std::exception("rvalue(VarType type): VT_STRING case not supported");
		break;
	}
}
void Variable::replace_array(const std::shared_ptr<Variable[]>& a_arr, const unsigned __int16 length) 
{ 
	if (length != numElements)
		std::cout << "resizing the array from " << numElements << " to " << length << '\n';

	arr = a_arr; 
	numElements = length;
}

void Variable::print(unsigned __int16 spaces) const
{
	if (!spaces++)
		std::cout << std::format("{}:\n", name);

	const auto ValueToString = [](const Variable& var) -> std::string
	{
		switch (var.type) {
		case VarType::VT_INT:
			return std::to_string(var.get_int());
			break;
		case VarType::VT_FLOAT:
			return std::to_string(var.get_float());
			break;
		case VarType::VT_STRING:
			return var.get_string();
			break;
		}

		return "";
	};

	std::string prefix;

	for (unsigned __int16 i = 0; i < spaces; i++) {
		prefix.push_back(' ');
		prefix.push_back(' ');

	}

	for (int i = 0; i < numElements; i++) {
		std::cout << std::format("{}[{}]: <{}> ({})\n", prefix, i, VarTypes[int(get_type())], ValueToString(this->arr[i]));
		arr[i].print(spaces + 1);
	}
	if(!numElements && !name.empty())
		std::cout << std::format("{}: <{}> ({})\n", prefix, VarTypes[int(get_type())], ValueToString(*this));


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
Variable* DeclareVariable(const std::string& name, const VarType type)
{
	std::cout << "pushing \"" << name<< "\" of type '" << VarTypes[int(type)] << "' to stack!\n";

	if (type == VarType::VT_INVALID) {
		throw std::exception("DeclareVariable(): impossible scenario!");
	}

	if (stack_variables.find(std::string(name)) != stack_variables.end()) {
		throw std::exception(std::format("the variable \"{}\" is already defined", name).c_str());
	}

	return &stack_variables.insert(std::make_pair(name, Variable(name, type))).first->second;
}
unsigned __int16 GetArrayDepth(const Variable* var)
{
	unsigned __int16 size = 0;
	auto ref = var->arr.get();
	while (ref) {
		size += 1;
		ref = ref->arr.get();
	}

	return size;

}
std::string Variable::s_getvariabletype() const
{
	const auto arr_as_text = [](const unsigned __int16 depth) 
	{
		auto copy = depth;
		std::string r;
		for (unsigned __int16 i = 0; i < depth; i++) {
			r += "[]";
		}
		return r;
	};

	return std::format("{}{}", VarTypes[int(get_type())], arr_as_text(GetArrayDepth(this)));

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