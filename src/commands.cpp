#include "commands.hpp"
#include "ShelExceptions.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>

void Commands::WriteToFile(const std::string &pathToFile, const std::string &data)
{
	std::ofstream file(pathToFile, std::ios::binary | std::ios::trunc);

	if (!file.is_open())
	{
		throw CommandException("Open File " + pathToFile);
		return;
	}

	file.write(data.c_str(), data.size());
	if (!file)
	{
		throw CommandException("Write To File " + pathToFile);
		return;
	}
}

std::string Commands::ReadFromFile(const std::string &pathToFile)
{
	std::ifstream file(pathToFile, std::ios::binary);

	if (!file.is_open())
	{
		throw CommandException("Open File " + pathToFile);
		return "";
	}

	std::string content;
	char buffer[4096];

	while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0)
	{
		content.append(buffer, file.gcount());
	}

	return content;
}

void Commands::DeleteFile(const std::string &pathToFile)
{
	if (!std::filesystem::remove(pathToFile))
	{
		throw CommandException("Delete File" + pathToFile);
	}
}
void Commands::CopyFromTo(const std::string &src, const std::string &dest)
{
	std::filesystem::copy(src, dest);
}

void Commands::MoveFromTo(const std::string &src, const std::string &dest)
{
	std::error_code ec;
	if (std::filesystem::exists(dest, ec))
		throw CommandException("move " + src + " to " + dest + " (destination exists)");

	std::filesystem::rename(src, dest, ec);
	if (!ec) return;

	// in case rename fails
	std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive, ec);
	if (ec) throw CommandException("move " + src + " to " + dest + ": " + ec.message());

	std::filesystem::remove_all(src, ec);
	if (ec) throw CommandException("delete " + src + ": " + ec.message());
}
