#include "Sh-eL.hpp"
#include "ShelExceptions.hpp"
#include "commands.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <algorithm>
#include <string>
#include <variant>
#include <vector>

// write <data> <file>
static std::string CmdWrite(Shel &, const std::vector<std::string> &args)
{
	Commands::WriteToFile(args[1], args[0]);
	return {};
}

// read <file>
static std::string CmdRead(Shel &, const std::vector<std::string> &args)
{
	return Commands::ReadFromFile(args[0]);
}

// copy <src> <dest>
static std::string CmdCopy(Shel &, const std::vector<std::string> &args)
{
	Commands::CopyFromTo(args[0], args[1]);
	return {};
}

// move <src> <dest>
static std::string CmdMove(Shel &, const std::vector<std::string> &args)
{
	Commands::MoveFromTo(args[0], args[1]);
	return {};
}
// del <path>
static std::string CmdDel(Shel &, const std::vector<std::string> &args)
{
	Commands::DeleteFile(args[0]);
	return {};
}

// echo <text>
static std::string CmdEcho(Shel &, const std::vector<std::string> &args)
{
	return args[0];
}
// upper <text>
static std::string CmdUpper(Shel &, const std::vector<std::string> &args)
{
	std::string text = args[0];
	for (char &c : text) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
	return text;
}

// lower <text>
static std::string CmdLower(Shel &, const std::vector<std::string> &args)
{
	std::string text = args[0];
	for (char &c : text) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	return text;
}

// count <text>   number of lines
static std::string CmdCount(Shel &, const std::vector<std::string> &args)
{
	const std::string &text = args[0];
	std::size_t lines = static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));
	if (!text.empty() && text.back() != '\n') lines++; // last line without a trailing newline
	return std::to_string(lines);
}

Shel::Shel(std::function<void(std::string_view)> out)
    : m_commands{{"write", {2, &CmdWrite}}, {"read", {1, &CmdRead}},   {"copy", {2, &CmdCopy}},
                 {"move", {2, &CmdMove}},   {"del", {1, &CmdDel}},     {"echo", {1, &CmdEcho}},
                 {"upper", {1, &CmdUpper}}, {"lower", {1, &CmdLower}}, {"count", {1, &CmdCount}}},
      redirectFunc(std::move(out))
{
}

void Shel::RunCommand(std::string cmd)
{
	Parser::Pipeline pipeline = Parser::Parse(Lexer::scan(cmd));

	Validate(pipeline); // check the whole tree first, so nothing runs if any part is wrong
	std::string out = RunPipeline(pipeline);
	if (!out.empty()) redirectFunc(out);
}

// Checks every command in the pipeline, including the ones inside ( ), without running anything
void Shel::Validate(const Parser::Pipeline &pipeline) const
{
	for (std::size_t i = 0; i < pipeline.size(); i++)
	{
		const Parser::Command &command = pipeline[i];

		auto it = m_commands.find(command.name);
		if (it == m_commands.end()) throw SyntaxException("unknown command '" + command.name + "'");

		// a command after a '|' also receives the previous command's output
		std::size_t given = command.args.size() + (i > 0 ? 1 : 0);
		if (given != it->second.argc)
			throw SyntaxException("'" + command.name + "' expects " + std::to_string(it->second.argc) + " argument(s)");

		for (const Parser::Arg &arg : command.args)
			if (const auto *inner = std::get_if<Parser::Pipeline>(&arg)) Validate(*inner);
	}
}

// Runs the pipeline left to right and returns the last command's output.
std::string Shel::RunPipeline(const Parser::Pipeline &pipeline)
{
	std::string out{};
	for (std::size_t i = 0; i < pipeline.size(); i++)
	{
		const Parser::Command &command = pipeline[i];

		std::vector<std::string> args{};
		if (i > 0) args.push_back(std::move(out)); // the previous command's output comes first

		for (const Parser::Arg &arg : command.args)
		{
			if (const auto *text = std::get_if<std::string>(&arg))
				args.push_back(*text);
			else
				args.push_back(RunPipeline(std::get<Parser::Pipeline>(arg))); // run ( ... ) first
		}
		out = m_commands.at(command.name).run(*this, args);
	}
	return out;
}
