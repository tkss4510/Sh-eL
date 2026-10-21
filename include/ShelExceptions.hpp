#ifndef SHELEXCEPTIONS_HPP
#define SHELEXCEPTIONS_HPP

#include <exception>
#include <string>

class SyntaxException : public std::exception
{
  private:
	std::string msg;

  public:
	SyntaxException(std::string _msg) : msg(_msg) {}
	virtual const char *what() const throw()
	{
		static std::string result;
		result = (std::string("Could Not Parse Given Syntax: ") + msg);
		return result.c_str();
	}
};

class TokenizerException : public std::exception
{
  private:
	int pos;

  public:
	TokenizerException(int _pos) : pos(_pos) {}
	virtual const char *what() const throw()
	{
		static std::string result;
		result = (std::string("Could Not Tokenize At Position: ") + std::to_string(pos));
		return result.c_str();
	}
};

class NullInputException : public std::exception
{
  public:
	virtual const char *what() const throw() { return "Input Null or Empty"; }
};

class CommandException : public std::exception
{
  private:
	std::string msg;

  public:
	CommandException(std::string _msg) : msg("Failed to execute " + _msg) {}
	const char *what() const noexcept override { return msg.c_str(); }
};

class MathException : public std::exception
{
  private:
	std::string msg;

  public:
	MathException(std::string _msg) : msg("Math Error: " + _msg) {}
	const char *what() const noexcept override { return msg.c_str(); }
};

#endif // !SHELEXCEPTIONS_HPP
