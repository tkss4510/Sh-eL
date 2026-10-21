#ifndef SHEL_HPP
#define SHEL_HPP

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace Parser
{
struct Command;
}

class Shel
{
  private:
	struct CommandSpec
	{
		std::size_t argc;
		std::string (*run)(Shel &, const std::vector<std::string> &args);
	};

	std::string m_cmd;
	const std::map<std::string, CommandSpec> m_commands;
	std::function<void(std::string_view)> redirectFunc; // Users pass this to redirect output

	void Validate(const std::vector<Parser::Command> &pipeline) const;
	std::string RunPipeline(const std::vector<Parser::Command> &pipeline);

  public:
	explicit Shel(std::function<void(std::string_view)> out);
	void RunCommand(std::string cmd);
};

#endif
