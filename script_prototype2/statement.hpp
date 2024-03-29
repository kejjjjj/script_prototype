
#ifndef __statm
#define __statm

#include "pch.hpp"
#include "script.hpp"

class code_segment
{
	code_segment(scr_scope_t* _scope, const code_segment_t& _code) : 
		scope(_scope), 
		code(_code)
	{
	}

	virtual void evaluate() = 0;

private:
	scr_scope_t* scope;
	code_segment_t code;
};

class statementKeywordTable
{
public:
	static statementKeywordTable& getInstance() {
		static statementKeywordTable instance;
		return instance;
	}

	auto find_builtin(const std::string& v) const {
		auto found = std::find_if(built_in.begin(), built_in.end(), [&v](const std::pair<std::string, tokenType>& str) { return !v.compare(str.first); });
		return found != built_in.end() ? std::make_optional(std::make_pair(found, std::distance(built_in.begin(), found))) : std::nullopt;
	}

	auto& get_list() const noexcept(true) {
		return built_in;
	};

private:
	const std::list<std::pair<std::string, tokenType>> built_in = {
		{"if", tokenType::IF_STATEMENT },
		{"else", tokenType::ELSE_STATEMENT },
		{"fn", tokenType::FUNCTION },
		{"return", tokenType::RETURN},


	};

	
};
enum class statementType_e : char
{
	EXPRESSION,
	DECLARATION,
	IF_STATEMENT,
	//ELSE_IF_STATEMENT,
	ELSE_STATEMENT,
	SCOPE,
	SCOPE_EXIT,
	FUNCTION_DECLARATION,
	RETURN
};



statementType_e statement_resolve_datatype_ambiguity(code_segment_t& tokens);
statementType_e statement_determine(const code_segment_t& tokens);

#endif