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
std::optional<code_segment_t> find_matching_square_bracket(const code_segment_t& token)
{
	code_segment_t copy = token;

	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_OPEN;
		};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_CLOSE;
		};

	if (is_opening(*copy.it) == false)
		throw scriptError_t(&*copy.it, "expected to find a \"[\"\n");

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
	throw scriptError_t(&*copy.it, "expected to find a \"]\"");
}
std::optional<code_segment_t> find_appropriate_comma_delimiter(const code_segment_t& token)
{
	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
	};

	const auto is_square_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_OPEN == true ?
			 std::make_optional(find_matching_square_bracket) : std::nullopt;
	};

	const auto is_opening_parenthesis = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_PAR_OPEN == true ?
			 std::make_optional(find_matching_parenthesis) : std::nullopt;
	};

	const auto is_curly_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN == true ?
			std::make_optional(find_matching_curly_bracket) : std::nullopt;
	};

	const auto test_punctuations = [&](const token_t& token) {
		if (auto v = is_opening_parenthesis(token))
			return v;

		if(auto v = is_square_bracket(token))
			return v;

		return is_curly_bracket(token);

	};

	code_segment_t copy = token;
	auto start = copy.it;
	size_t numIterations = 0;


	while (copy.it != copy.end) {
		
		if (auto func = test_punctuations(*copy.it)) {
			auto substr = func.value()(copy);
			LOG("PARENTHESES\n");
			if (substr.has_value()) {
				auto& v = substr.value();

				copy.it = v.end;
				copy.it++; //go to the punctuation character
				//copy.it++; //skip the character

				if(copy.it == copy.end)
					return code_segment_t{ start, start, copy.it };

			}

		}
		if (is_comma(*copy.it)) {
			if (numIterations == 0)
				return std::nullopt;

			return code_segment_t{ start, start, copy.it };
		}

		copy.it++;
		numIterations++;

	}

	if (numIterations == 0)
		return std::nullopt;

	return code_segment_t{ start, start, --copy.it };


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