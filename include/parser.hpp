#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include <string>
#include <variant>
#include <vector>

namespace Parser
{
struct Command;
using Pipeline = std::vector<Command>;
using Arg = std::variant<std::string, Pipeline>; // a quoted string, or a ( pipeline ) to run first

struct Command
{
	std::string name{};
	std::vector<Arg> args{};
};

// line     = pipeline eof
// pipeline = command ( '|' command )*
// command  = WORD arg*
// arg      = STRING | '(' pipeline ')' | '(' expr ')'    an expr is computed here and becomes a string
// expr     = term ( ( '+' | '-' ) term )*
// term     = factor ( ( '*' | '/' ) factor )*
// factor   = number | '-' factor | '(' expr ')'
Pipeline Parse(const std::vector<Lexer::Token> &tokens);
} // namespace Parser

#endif
