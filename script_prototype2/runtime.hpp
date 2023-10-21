#pragma once

#include "script.hpp"
#include "scope.hpp"

template <typename... Args>
void r_print(Args... args) {
	(std::cout << ... << args) << '\n';
}

class runtimeError_t
{
public:
	runtimeError_t(const std::string& reason) : errReason(reason) {

	}
	runtimeError_t(const token_t* _token, const std::string& reason) : token(_token), errReason(reason) {

		errReason = std::format("runtime error:\n{}\non line: [{},{}]", errReason, token->line, token->column);

	};
	runtimeError_t(const script_t* _script, const std::string& reason) : script(_script), errReason(reason) {

		errReason = std::format("runtime error:\n{}\non line: [{},{}]", errReason, script->line, script->column);

	};
	runtimeError_t() = delete;
	[[nodiscard]] const char* const what() const noexcept {
		return errReason.empty() ? "null exception" : errReason.c_str();
	}

	std::string errReason;
	const script_t* script = 0;
	const token_t* token = 0;

};
