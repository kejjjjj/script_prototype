#include "parsing_utils.hpp"

std::optional<code_segment_t> find_matching_parenthesis(const code_segment_t& token)
{
	code_segment_t copy = token;

	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_PAR_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_PAR_CLOSE;
	};

	if (is_opening(*copy.it) == false)
		throw scriptError_t(&*copy.it, "expected to find a \"(\"\n");

	copy.it++;
	auto begin = copy.it;

	size_t numOpen = 1;
	size_t numClosing = 0;

	size_t numIterations = 0;

	while (copy.it != copy.end) {
		if (is_opening(*copy.it))
			numOpen++;

		else if (is_closing(*copy.it)) {
			numClosing++;

			if (numOpen == numClosing) {

				if (numIterations == NULL)
					return std::nullopt;

				return std::make_optional<code_segment_t>({ begin, begin , --copy.it });
			}
		}

		copy.it++;
		numIterations++;
	}
	throw scriptError_t(&*copy.it, "expected to find a \")\"");
}

std::optional<code_segment_t> find_matching_curly_bracket(const code_segment_t& token)
{
	code_segment_t copy = token;

	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_CLOSE;
	};

	if (is_opening(*copy.it) == false)
		throw scriptError_t(&*copy.it, "expected to find a \"{\"\n");

	copy.it++;
	auto begin = copy.it;

	size_t numOpen = 1;
	size_t numClosing = 0;

	size_t numIterations = 0;

	while (copy.it != copy.end) {
		if (is_opening(*copy.it))
			numOpen++;

		else if (is_closing(*copy.it)) {
			numClosing++;

			if (numOpen == numClosing) {

				if (numIterations == NULL)
					return std::nullopt;

				return std::make_optional<code_segment_t>({ begin, begin , --copy.it });
			}
		}

		copy.it++;
		numIterations++;
	}
	throw scriptError_t(&*copy.it, "expected to find a \"}\"");
}
std::optional<code_segment_t> find_next_closing_curly_bracket(const code_segment_t& token)
{
	code_segment_t copy = token;

	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_CLOSE;
	};

	auto begin = copy.it;

	size_t numOpen = 1;
	size_t numClosing = 0;

	size_t numIterations = 0;

	while (copy.it != copy.end) {
		if (is_opening(*copy.it))
			numOpen++;

		else if (is_closing(*copy.it)) {
			numClosing++;

			if (numOpen == numClosing) {

				if (numIterations == NULL)
					return std::nullopt;

				return std::make_optional<code_segment_t>({ begin, begin , --copy.it });
			}
		}

		copy.it++;
		numIterations++;
	}
	throw scriptError_t(&*copy.it, "expected to find a \"}\"");
}