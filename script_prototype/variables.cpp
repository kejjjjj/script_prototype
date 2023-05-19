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
		value.buffer = std::make_shared<char*>(new char[4]);
		value.buf_size = sizeof(int);
		*reinterpret_cast<int*>(value.buffer.get()) = 0;
		break;
	case VarType::VT_FLOAT:
		value.buffer = std::make_shared<char*>(new char[4]);
		value.buf_size = sizeof(float);
		*reinterpret_cast<float*>(value.buffer.get()) = 0;

		break;
	case VarType::VT_STRING:
		value.buffer = std::make_shared<char*>(new char[1]);
		value.buf_size = sizeof(float);
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
void Variable::set_value(const expr::expression_token* token)
{
	//assumes that the types are compatible

	bool const rvalue = token->is_rvalue();
	auto const rtype = token->get_type();

	auto ptr = this;
	auto token_ptr = token;

	if (is_reference())
		ptr = this->reference.get();

	if (token->is_lvalue()) {
		if (token->lval->ref->is_reference())
			token_ptr = token->lval->ref->reference.get();
	}

	if (is_array()) {
		ptr->replace_array(token->lval->ref->arr, token->lval->ref->numElements);
		return;
	}

	switch (ptr->type) {
	case VarType::VT_INT:

		if (rtype == VarType::VT_FLOAT)
			ptr->set_value<int>(token->get_float());
		else if (rtype == VarType::VT_INT)
			ptr->set_value<int>(token->get_int());

		break;
	case VarType::VT_FLOAT:

		if (rtype == VarType::VT_FLOAT)
			ptr->set_value<float>(token->get_float());
		else if (rtype == VarType::VT_INT)
			ptr->set_value<float>(token->get_int());

		break;
	case VarType::VT_STRING:
		if (token->is_lvalue())
			ptr->set_string(token->get_string());
		else {
			const std::string str = token->rval->get_string();
			ptr->set_string((char*)str.substr(1, str.size() - 2).c_str()); //remove the quotation marks
		}
		break;
	}

}
void Variable::print(unsigned __int16 spaces) const
{
	if (!spaces++)
		std::cout << std::format("{}:\n", name);

	const auto ValueToString = [](const Variable& var) -> std::string
	{
		switch (var.type) {
		case VarType::VT_INT:
			if(var.is_reference())
				return std::to_string(var.reference->get_int());

			return std::to_string(var.get_int());
			break;
		case VarType::VT_FLOAT:
			if (var.is_reference())
				return std::to_string(var.reference->get_float());
			return std::to_string(var.get_float());
			break;
		case VarType::VT_STRING:
			if (var.is_reference())
				return var.reference->get_string();
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
Variable::operator expr::expression_token()
{
	expr::expression_token token;
	token.content = name;
	token.lval = std::shared_ptr<lvalue>(new lvalue);
	token.lval->ref = this;
	token.tokentype = type;

	return token;
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
	std::function<std::string(const Variable*)> types_to_text = [&types_to_text](const Variable* var) -> std::string {

		if (!var)
			return "";

		if (var->is_reference())
			return "?" + types_to_text(var->reference.get());

		if (var->is_array())
			return "[]" + types_to_text(var->arr.get());

		return "";

	};

	const auto arr_as_text = [](const unsigned __int16 depth) 
	{
		auto copy = depth;
		std::string r;
		for (unsigned __int16 i = 0; i < depth; i++) {
			r += "[]";
		}
		return r;
	};

	return std::format("{}{}", VarTypes[int(get_type())], types_to_text(this));

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