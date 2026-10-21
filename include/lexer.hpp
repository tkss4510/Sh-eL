#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <string_view>
#include <vector>

namespace Lexer
{
enum class TokenType
{
	pipe, // | used for passing input from command on left to command on the right
	leftParen,
	rightParen,

	number,
	plus,
	minus,
	asterisk,
	slash,

	word,
	string,
	eof,

};

struct Token
{
	TokenType type = TokenType::eof;
	std::string value;
	Token() = default;
	Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};

std::vector<Token> scan(std::string_view line);
} // namespace Lexer

#endif
