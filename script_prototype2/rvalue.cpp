#include "rvalue.hpp"

rvalue::rvalue(const rval_declr_data& declaration_data)
{
	type = declaration_data.type;
	token = *declaration_data.token;

	switch (type) {
	case dataTypes_e::CHAR:
		value.buffer = std::make_unique<char*>(new char[sizeof(char)]);
		value.buf_size = sizeof(char);
		break;
	case dataTypes_e::INT:
		value.buffer = std::make_unique<char*>(new char[sizeof(int32_t)]);
		value.buf_size = sizeof(int32_t);
		break;
	case dataTypes_e::FLOAT:
		value.buffer = std::make_unique<char*>(new char[sizeof(float)]);
		value.buf_size = sizeof(float);
		break;

	default:
		throw scriptError_t("huh.. rvalue constructed without a type..");

	}
}

void rvalue::set_initial_value(const std::string& s) 
{

	size_t accumulatedValue = 0;

	try {
		switch (get_type()) {
		case dataTypes_e::CHAR:

			if (token.tt == tokenType::NUMBER_LITERAL) {
				accumulatedValue = std::stoi(std::string(s.begin(), --s.end()));
				
				if (accumulatedValue > 127)
					throw scriptError_t(&token, "literal overflow");

				set_value<char>(static_cast<char>(accumulatedValue));
				break;
			}

			if (std::distance(s.begin(), s.end()) < 2) {
				throw scriptError_t(&token, "empty char literal is not allowed");
			}
			accumulatedValue = std::accumulate(++s.begin(), --s.end(), 0ull);

			if(accumulatedValue > 127)
				throw scriptError_t(&token, "literal overflow");

			set_value<char>(static_cast<char>(accumulatedValue));
			break;
		case dataTypes_e::INT:
			set_value<int>(std::stoi(s));
			break;
		case dataTypes_e::FLOAT:
			set_value<float>(std::stof(s));
			break;
		}
	}
	catch (std::out_of_range&) {
		throw scriptError_t(&token, "literal overflow");
	}
	catch (std::exception& ex) {
		throw scriptError_t(&token, ex.what());
	}
}