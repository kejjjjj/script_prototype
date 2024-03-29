#pragma once

#include "Variable.hpp"
#include "functions.hpp"
#include "statement.hpp"
struct codepos_t
{
	size_t line;
	size_t column;
};

struct function_scope;

struct function_table_s
{
	size_t numArgs = 0;
	std::list<datatype_declaration> args;
	datatype_declaration return_datatype;
};
struct function_scope
{
	std::string identifier;
	size_t numArgs = 0;
	std::list<datatype_declaration> param_types;
	datatype_declaration return_datatype;
	bool function_will_return = false;
};

class scr_scope_t
{
public:

	scr_scope_t() {
		localVars = std::unique_ptr<VariableTable>(new VariableTable);
	};
	scr_scope_t(const scr_scope_t&) = delete;
	scr_scope_t& operator=(const scr_scope_t&) = delete;

	void set_lower_scope(const scr_scope_t* scope) noexcept {
		lower_scope = const_cast<scr_scope_t*>(scope); 

	}
	auto get_lower() noexcept { return lower_scope; }
	void emit_to_lower_scope(const statementType_e type) noexcept { 
		if(lower_scope)
			lower_scope->upper_scope_type = type; 
	}
	//~scr_scope_t() { on_exit(); }
	bool is_global_scope() const noexcept { return lower_scope == nullptr; }

	void make_function_scope(std::unique_ptr<function_scope>& function)
	{
		function_scope = std::move(function);
	};
	function_scope* is_function_scope() noexcept {
		
		if (is_global_scope())
			return nullptr;

		scr_scope_t* s = this;
		do {

			if (s->function_scope)
				return s->function_scope.get();
			
			s = s->get_lower();
		} while (s->is_global_scope() == false);

		return nullptr;
	}

	decltype(auto) declare_variable(std::unique_ptr<Variable>&& v) {
		return localVars->declare_variable((v));
	}
	Variable* find_variable(const std::string& v) {

		if (auto var = localVars->find(v))
			return var.value()->second.get();

		if (is_global_scope())
			return nullptr;

		return lower_scope->find_variable(v);
		

	}

	void set_function_table(std::unordered_map<std::string, function_table_s>* table) { function_table = table; }
	decltype(auto) get_function_table() noexcept { return function_table; }

	void add_to_function_table(const function_scope& data) {
		function_table->insert({
			data.identifier,
			function_table_s
			{
				.numArgs = data.numArgs,
				.args = data.param_types,
				.return_datatype = data.return_datatype
			} });
	}

	std::optional<statementType_e> get_scope_context() const noexcept { return upper_scope_type; }

	void set_range(codepos_t&& begin, codepos_t&& end) noexcept {
		codepos_begin = std::move(begin);
		codepos_end = std::move(end);

		std::cout << std::format("scope bounds: [{}, {}] to [{}, {}]\n", codepos_begin.line, codepos_begin.column, codepos_end.line, codepos_end.column);

	}

	void print_localvars() const noexcept {
		localVars->print();
	}

	auto on_exit(){


		LOG("exiting scope\n");
		print_localvars();
		localVars->erase_all();

		return lower_scope;
	}
private:


	codepos_t codepos_begin{};
	scr_scope_t* lower_scope = 0;
	std::unique_ptr<VariableTable> localVars;
	codepos_t codepos_end{};
	std::unique_ptr<function_scope> function_scope;
	std::optional<statementType_e> upper_scope_type;
	std::unordered_map<std::string, function_table_s>* function_table = 0;

};

void Codeblock_read(script_t& script, scr_scope_t** block);

scr_scope_t* create_scope_without_range(scr_scope_t* block);
scr_scope_t* create_scope(script_t& script, scr_scope_t* block);
void set_range_for_scope(script_t& script, scr_scope_t* scope);
scr_scope_t* delete_scope(script_t& script, scr_scope_t* block);