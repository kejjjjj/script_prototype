#include "script.hpp"
#include "filetools.hpp"

script_t::script_t(const std::string& filename)
{

	std::string path = filename;
	std::fstream f;
	path = fs::get_root_directory() + "\\" + path;
	std::cout << "path: " << path << '\n';
	this->filename = filename;
	if (!fs::open_file(f, path, fs::fileopen::FILE_IN)) {
		return;
	}
	std::cout << std::format("\"{}\" successfully opened!\n", path);

	//read file to a string buffer
	char buf[4096];
	while (f.read(buf, sizeof(buf)))
		this->buffer.append(buf, sizeof(buf));
	this->buffer.append(buf, f.gcount());

	if (this->buffer.empty()) {
		fs::close_file(f);
		return;
	}

	fs::close_file(f);

	this->script_p = this->buffer.begin();
	this->lastscript_p = script_p;
	this->scriptend_p = this->buffer.end();
	this->line = 1ull;
	this->column = 1ull;
	
}
void script_t::S_Tokenize()
{
	token_t token;

	while (S_ReadToken(token)) {
		tokens.push_back(token);
		token.reset();
	}
	token_it = tokens.begin();
}
token_expression_t script_t::S_CreateExpression()
{
	auto& end = token_it;
	auto begin = token_it;

	while (end++ != tokens.end()) {

		if (end->tt == tokenType::PUNCTUATION && end->extrainfo == P_SEMICOLON)
			return { begin, begin, end };
	}

	throw scriptError_t(this, "unexpected end of file");
}
bool script_t::S_ReadToken(token_t& token)
{
	lastscript_p = script_p;

	if (!S_ReadWhitespace())
		return 0;

	token.line = line;
	token.column = column;

	if (std::isdigit(*script_p) || *script_p == '.' && std::isdigit(*(script_p + 1))) {
		if (!S_ReadNumber(token)) {
			return 0;
		}
	}
	else if (std::isalpha(*script_p) || *script_p == '_') {
		if (!S_ReadName(token)) {
			return 0;
		}
	}
	else {
		if (!S_ReadPunctuation(token)) {
			throw scriptError_t(this, "an unexpected token");
			return 0;
		}
	}

	return 1;
}

bool script_t::S_ReadWhitespace()
{
	while (*script_p <= ' ') {


		if (script_p == scriptend_p)
			return 0;

		if (*script_p == '\n') {
			line++;
			column = 1ull;
		}
		else {
			if (*script_p == '\t')
				column += 4;
			else
				column++;
		}

		script_p++;
	}

	return 1;
}
bool script_t::S_ParseInt(token_t& token)
{
	if (script_p == scriptend_p)
		return 0;

	if (std::isdigit(*script_p)) {
		token.string.push_back(*script_p++);

		if (script_p == scriptend_p)
			return 0;

		while (isdigit(*script_p)) {
			token.string.push_back(*script_p++);

			if (script_p == scriptend_p)
				return 0;

		}
	}

	return 1;

}
bool script_t::S_ReadNumber(token_t& token)
{
		
	//a floating point literal with . prefix
	if (*script_p == '.') { //assumes that there is a number next

		token.string.push_back(*script_p++);
		token.tt = tokenType::FLOAT_LITERAL;

		//parse the integer literal after the .
		if (!S_ParseInt(token))
			return 0;

	}

	//an integral literal
	else if(std::isdigit(*script_p)){
		token.tt = tokenType::NUMBER_LITERAL;

		if (!S_ParseInt(token))
			return 0;

		//floating point decimal
		if (*script_p == '.') { 
			token.string.push_back(*script_p++);
			token.tt = tokenType::FLOAT_LITERAL;

			//parse the integer literal after the .
			if (!S_ParseInt(token)) 
				return 0;
		}

		//parse suffix

	}
	


	token.print();
	column += token.string.length();
	return 1;
}
bool script_t::S_ReadName(token_t& token)
{
	token.tt = tokenType::NAME;
	token.string.push_back(*script_p++);

	while (std::isalnum(*script_p) || *script_p == '_') {

		if (script_p == scriptend_p)
			return 0;

		token.string.push_back(*script_p++);
	}

	token.print();
	column += token.string.length();
	return 1;
}
bool script_t::S_ReadPunctuation(token_t& token)
{
	for (const auto& i : punctuations) {

		//check eof
		if ((script_p + i.identifier.length()-1ull) != scriptend_p) {

			
			const std::string parsed_str = std::string(script_p, (script_p + i.identifier.length())); 

			if (!parsed_str.compare(i.identifier)) {
				token.string = i.identifier;
				token.tt = tokenType::PUNCTUATION;

				MAKE_LOWORD(token.extrainfo, static_cast<WORD>(i.punc));
				MAKE_HIWORD(token.extrainfo, static_cast<DWORD>(i.priority));


				script_p += token.string.length();

				token.print();
				column += token.string.length();

				

				return 1;
			}

			
		}

	}
	return 0;
}