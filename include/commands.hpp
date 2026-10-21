#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>

namespace Commands
{
void WriteToFile(const std::string &pathToFile, const std::string &data);
std::string ReadFromFile(const std::string &pathToFile);
void DeleteFile(const std::string &pathToFile);
void CopyFromTo(const std::string &src, const std::string &dest);
void MoveFromTo(const std::string &src, const std::string &dest);

} // namespace Commands

#endif // COMMANDS_HPP
