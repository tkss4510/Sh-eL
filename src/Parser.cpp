#include "parser.hpp"
#include "ShelExceptions.hpp"
#include "math.hpp"
#include <charconv>
#include <cstddef>

namespace Parser
{
namespace
{
constexpr int MaxDepth = 64; // how deeply ( ) and unary '-' may nest

struct Context
{
	const std::vector<Lexer::Token> &tokens;
	std::size_t i = 0;
	int depth = 0;
	const Lexer::Token &cur() const { return tokens[i]; }
	const Lexer::Token &peek() const { return i + 1 < tokens.size() ? tokens[i + 1] : tokens.back(); }
};

// Counts one level of nesting for as long as it is alive, so deep input throws
struct DepthGuard
{
	Context &ctx;
	explicit DepthGuard(Context &c) : ctx(c)
	{
		if (ctx.depth >= MaxDepth) throw SyntaxException("nested too deeply");
		ctx.depth++;
	}
	~DepthGuard() { ctx.depth--; }
};

Pipeline ParsePipeline(Context &ctx);
Command ParseCommand(Context &ctx);
Arg ParseArg(Context &ctx);
long long ParseExpr(Context &ctx);
long long ParseTerm(Context &ctx);
long long ParseFactor(Context &ctx);

// Stops at the first token that is not a '|'
Pipeline ParsePipeline(Context &ctx)
{
	Pipeline pipeline{};
	while (true)
	{
		pipeline.push_back(ParseCommand(ctx));
		if (ctx.cur().type != Lexer::TokenType::pipe) break;
		ctx.i++; // consume '|', another command must follow
	}
	return pipeline;
}

Command ParseCommand(Context &ctx)
{
	if (ctx.cur().type != Lexer::TokenType::word) throw SyntaxException("expected a command name");
	Command cmd{ctx.cur().value, {}};
	ctx.i++;

	while (ctx.cur().type == Lexer::TokenType::string || ctx.cur().type == Lexer::TokenType::leftParen)
		cmd.args.push_back(ParseArg(ctx));
	return cmd;
}

// STRING | '(' pipeline ')' | '(' expr ')'
Arg ParseArg(Context &ctx)
{
	if (ctx.cur().type == Lexer::TokenType::string)
	{
		Arg arg = ctx.cur().value;
		ctx.i++;
		return arg;
	}

	// cur is ( if a word is next then another pipeline otherwise mathematical expression
	if (ctx.peek().type == Lexer::TokenType::word)
	{
		DepthGuard guard(ctx);
		ctx.i++; // consume '('
		Pipeline inner = ParsePipeline(ctx);
		if (ctx.cur().type != Lexer::TokenType::rightParen) throw SyntaxException("expected ')'");
		ctx.i++; // consume ')'
		return inner;
	}

	ctx.i++; // consume '('
	long long value = ParseExpr(ctx);
	if (ctx.cur().type != Lexer::TokenType::rightParen) throw SyntaxException("expected ')'");
	ctx.i++; // consume ')'
	return std::to_string(value);
}

// term ( ( '+' | '-' ) term )*
long long ParseExpr(Context &ctx)
{
	long long left = ParseTerm(ctx);
	while (ctx.cur().type == Lexer::TokenType::plus || ctx.cur().type == Lexer::TokenType::minus)
	{
		bool add = ctx.cur().type == Lexer::TokenType::plus;
		ctx.i++;
		long long right = ParseTerm(ctx);
		left = add ? Math::Add(left, right) : Math::Sub(left, right);
	}
	return left;
}

// factor ( ( '*' | '/' ) factor )*
long long ParseTerm(Context &ctx)
{
	long long left = ParseFactor(ctx);
	while (ctx.cur().type == Lexer::TokenType::asterisk || ctx.cur().type == Lexer::TokenType::slash)
	{
		bool multiply = ctx.cur().type == Lexer::TokenType::asterisk;
		ctx.i++;
		long long right = ParseFactor(ctx);
		left = multiply ? Math::Mul(left, right) : Math::Div(left, right);
	}
	return left;
}

// NUMBER | '-' factor | '(' expr ')'
long long ParseFactor(Context &ctx)
{
	DepthGuard guard(ctx);
	const Lexer::Token &token = ctx.cur();

	if (token.type == Lexer::TokenType::number)
	{
		long long value = 0;
		auto result = std::from_chars(token.value.data(), token.value.data() + token.value.size(), value);
		if (result.ec != std::errc{}) throw MathException("number too large: " + token.value);
		ctx.i++;
		return value;
	}

	if (token.type == Lexer::TokenType::minus)
	{
		ctx.i++;
		return Math::Sub(0, ParseFactor(ctx));
	}

	if (token.type == Lexer::TokenType::leftParen)
	{
		ctx.i++;
		long long value = ParseExpr(ctx);
		if (ctx.cur().type != Lexer::TokenType::rightParen) throw SyntaxException("expected ')'");
		ctx.i++;
		return value;
	}

	throw SyntaxException("expected a number");
}
} // namespace

Pipeline Parse(const std::vector<Lexer::Token> &tokens)
{
	if (tokens.empty() || tokens.back().type != Lexer::TokenType::eof)
		throw SyntaxException("token list must end with eof");

	Context ctx{tokens};
	Pipeline pipeline = ParsePipeline(ctx);

	if (ctx.cur().type == Lexer::TokenType::rightParen) throw SyntaxException("unexpected ')'");
	if (ctx.cur().type != Lexer::TokenType::eof) throw SyntaxException("expected a quoted string, '(' or '|'");
	return pipeline;
}
} // namespace Parser
