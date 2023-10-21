
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

enum class statementKeywords_e : short
{
	IF
};

class statementKeywordTable
{
public:
	static statementKeywordTable& getInstance() {
		static statementKeywordTable instance;
		return instance;
	}

	auto find_builtin(const std::string& v) const {
		auto found = std::find_if(built_in.begin(), built_in.end(), [&v](const std::pair<std::string, statementKeywords_e>& str) { return !v.compare(str.first); });
		return found != built_in.end() ? std::make_optional(std::make_pair(found, std::distance(built_in.begin(), found))) : std::nullopt;
	}

	auto& get_list() const noexcept(true) {
		return built_in;
	};

private:
	const std::list<std::pair<std::string, statementKeywords_e>> built_in = {
		{"if", statementKeywords_e::IF },
	};

	
};
enum class statementType_e : char
{
	EXPRESSION,
	DECLARATION,
	STATEMENT_KEYWORD,
	SCOPE,
	SCOPE_EXIT,
	FUNCTION_DECLARATION
};



statementType_e statement_resolve_datatype_ambiguity(code_segment_t& tokens);
statementType_e statement_determine(const code_segment_t& tokens);

#endif