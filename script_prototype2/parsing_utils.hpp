#pragma once

#include "pch.hpp"
#include "script.hpp"

std::optional<code_segment_t> find_matching_parenthesis(const code_segment_t& token);
std::optional<code_segment_t> find_matching_curly_bracket(const code_segment_t& token);
std::optional<code_segment_t> find_next_closing_curly_bracket(const code_segment_t& token);