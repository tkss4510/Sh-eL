#include "lexer.hpp"
#include "ShelExceptions.hpp"
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

std::vector<Lexer::Token> Lexer::scan(std::string_view line)
{
	std::vector<Token> tokens;
	size_t pos = 0;
	auto get = [&]() -> char { return pos < line.size() ? line[pos] : '\0'; };

	while (get() != '\0')
	{
		if (std::isspace(get()))
		{
			pos++;
			continue;
		} // Skip White-space
		else if (get() == '\"') // String Must start with "" unless word
		{
			pos++;
			Token token;
			token.type = TokenType::string;
			while (get() != '\"' && get() != '\0') // Ignoring esacape string and just taking the value
			{
				token.value += get();
				pos++;
			}
			if (get() == '\"') pos++;
			if (token.value == "" || token.value == " ") continue;
			tokens.push_back(token);
			continue;
		}
		else if (get() == '|')
		{
			Token token;
			token.type = TokenType::pipe;
			token.value = '|';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (get() == '(')
		{
			Token token;
			token.type = TokenType::leftParen;
			token.value = '(';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (get() == ')')
		{
			Token token;
			token.type = TokenType::rightParen;
			token.value = ')';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (get() == '+')
		{
			Token token;
			token.type = TokenType::plus;
			token.value = '+';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (get() == '-')
		{
			Token token;
			token.type = TokenType::minus;
			token.value = '-';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (get() == '*')
		{
			Token token;
			token.type = TokenType::asterisk;
			token.value = '*';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (get() == '/')
		{
			Token token;
			token.type = TokenType::slash;
			token.value = '/';
			tokens.push_back(token);
			pos++;
			continue;
		}
		else if (std::isdigit(get())) // number
		{
			Token token;
			while (std::isdigit(get()))
			{
				token.value += get();
				pos++;
			}
			token.type = TokenType::number;
			tokens.push_back(token);
			continue;
		}
		else if (std::isalpha(get())) // Keyword
		{
			Token token;
			while (std::isalpha(get()))
			{
				token.value += get();
				pos++;
			}
			token.type = TokenType::word;
			tokens.push_back(token);
			continue;
		}
		else
		{
			throw TokenizerException(pos);
		}
	}
	pos++;
	tokens.push_back(Token(TokenType::eof, "/0"));
	return tokens;
}
