#include "pch.h"


Variable::Variable(const std::string_view& _name, VarType _type) : name(_name), type(_type){
	AllocateValues();
}

void Variable::AllocateValues() {
	switch (type) {
	case VarType::VT_INT:
		value.buffer = std::make_shared<char*>(new char[4]);
		value.buf_size = sizeof(int);
		set_int(0);
		break;
	case VarType::VT_FLOAT:
		value.buffer = std::make_shared<char*>(new char[4]);
		value.buf_size = sizeof(float);
		set_float(0);
		break;
	case VarType::VT_STRING:
		value.buffer = std::make_shared<char*>(new char[1]);
		value.buf_size = sizeof(float);
		break;
	case VarType::VT_CHAR:
		value.buffer = std::make_shared<char*>(new char[1]);
		value.buf_size = sizeof(char);
		break;
	}

}
void Variable::replace_array(const std::shared_ptr<Variable[]>& a_arr, const size_t length)
{ 
	if (length != numElements)
		std::cout << "resizing the array from " << numElements << " to " << length << '\n';

	arr = a_arr; 
	numElements = length;
}
void Variable::recreate_array(const size_t new_length)
{
	if (!is_array())
		throw std::exception("attempted to recreate a non-array type");

	arr.reset();
	arr = std::shared_ptr<Variable[]>(new Variable[new_length]);
}
size_t Variable::pointer_depth() const
{
	Variable* p = pointer.get();
	//Variable* a = arr.get();
	decltype(pointer_depth()) size{ 0 };

	while (p /*|| a*/) {
		++size;

		if (p) {
			p = p->pointer.get();
			break;
		}
		
		//a = a->arr.get();
	}

	return size;
}
size_t Variable::array_depth() const
{
	Variable* a = arr.get();
	decltype(pointer_depth()) size{ 0 };

	while (a) {
		++size;

		a = a->arr.get();
	}

	return size;
}
void Variable::set_expression(expr::expression_token* token)
{
	if (!ExpressionCompatibleOperands(*this, *token)) {
		throw std::exception(std::format("an operand of type \"{}\" is not compatible with \"{}\"", VarTypes[int(this->get_type())], VarTypes[int(token->tokentype)]).c_str());
	}

	auto ptr = this;

	//if (is_reference())
	//	ptr = this->reference.get();

	if (is_array()) {
		ptr->replace_array(token->lval->ref->arr, token->lval->ref->numElements);
		return;
	}

	expr::ExpressionMakeRvalue(*token);

	expr::expression_token this_token = *ptr;

	expr::ExpressionImplicitCast(this_token, *token);

	switch (ptr->type) {
	case VarType::VT_INT:

		ptr->set_value<int>(token->get_int());
		
		std::cout << "new value: " << ptr->get_int() << '\n';

		break;
	case VarType::VT_FLOAT:

		ptr->set_value<float>(token->get_float());

		std::cout << "new value: " << ptr->get_float() << '\n';


		break;
	case VarType::VT_STRING:
		if (token->is_lvalue())
			ptr->set_string(token->get_string());
		else {
			const std::string str = token->rval->get_string();
			ptr->set_string((char*)str.substr(1, str.size() - 2).c_str()); //remove the quotation marks
		}
		break;
	case VarType::VT_CHAR:
		ptr->set_value<char>(token->get_char());
		break;
	}

}
void Variable::initialize_expression(expr::expression_token* token)
{
	
/*	if (is_pointer() && !token->is_lvalue()) {
		stack_variables.erase(name);
		throw std::exception("pointer type initializer must be an lvalue");
	}
	else if (is_pointer() && token->is_lvalue()) {

		const auto left_type = s_getvariabletype();
		const auto right_type = token->lval->ref->s_getvariabletype();

		if (get_type() != token->get_type()) {
			throw std::exception(std::format("a reference of type \"{}\" cannot be initialized with type \"{}\"", left_type, right_type).c_str());
			stack_variables.erase(name);

		}
		pointer = std::shared_ptr<Variable>(token->lval->ref);
		std::cout << std::format("\"{}\" reference updated! (now points to \"{}\")\n", name, pointer->name);
	}*/



	set_expression(token);
}
void Variable::print(size_t spaces) const
{


	if (!spaces++) {
		std::cout << std::format("{}{}:\n", name, is_pointer() ? " -> " + pointer->name : "");
	}
	const auto ValueToString = [](const Variable& var) -> std::string
	{
		switch (var.type) {
		case VarType::VT_INT:
			if(var.is_pointer())
				return std::to_string(var.pointer->get_int());

			return std::to_string(var.get_int());
		case VarType::VT_FLOAT:
			if (var.is_pointer())
				return std::to_string(var.pointer->get_float());
			return std::to_string(var.get_float());
		case VarType::VT_STRING:
			if (var.is_pointer())
				return var.pointer->get_string();
			return var.get_string();
		case VarType::VT_CHAR:
			std::string s;
			if (var.is_pointer())
				return s.push_back(var.pointer->get_char()), s;
			return s.push_back(var.get_char()), s;
		}

		return "";
	};

	std::string prefix;

	for (size_t i = 0ull; i < spaces; i++) {
		prefix.push_back(' ');
		prefix.push_back(' ');

	}

	for (size_t i = 0; i < numElements; i++) {
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

		if (var->is_pointer())
			return "^" + types_to_text(var->pointer.get());

		if (var->is_array())
			return "[]" + types_to_text(var->arr.get());

		return "";

	};

	return std::format("{}{}", VarTypes[int(get_type())], types_to_text(this));

}