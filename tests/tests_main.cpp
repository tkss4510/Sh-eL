#include "Sh-eL.hpp"
#include "ShelExceptions.hpp"
#include <iostream>
#include <string>

int passed = 0;
int failed = 0;

// runs one command and returns whatever it printed
std::string Run(const std::string &line)
{
	std::string output;
	Shel shell([&](std::string_view text) { output = std::string(text); });
	shell.RunCommand(line);
	return output;
}

// given command and expected runs cmd and increments pass/fail
void ExpectOutput(const std::string &line, const std::string &expected)
{
	std::string actual = Run(line);
	if (actual == expected)
	{
		passed++;
	}
	else
	{
		failed++;
		std::cout << "FAIL: " << line << "\n";
		std::cout << "   expected: " << expected << "\n";
		std::cout << "   got:      " << actual << "\n";
	}
}

// given command runs cmd and increments pass/fail
void ExpectMathError(const std::string &line)
{
	try
	{
		Run(line);
	}
	catch (const MathException &)
	{
		passed++;
		return;
	}
	catch (...)
	{
	}
	failed++;
	std::cout << "FAIL: " << line << "   (expected a math error)\n";
}

// Test: the command should fail with a SyntaxException
void ExpectSyntaxError(const std::string &line)
{
	try
	{
		Run(line);
	}
	catch (const SyntaxException &)
	{
		passed++;
		return;
	}
	catch (...)
	{
	}
	failed++;
	std::cout << "FAIL: " << line << "   (expected a syntax error)\n";
}

int main()
{
	// the command, then what it should print
	ExpectOutput("echo \"hi\"", "hi");
	ExpectOutput("echo \"hi\" | upper", "HI");
	ExpectOutput("upper (echo \"abc\")", "ABC");
	ExpectOutput("echo (1+2*3)", "7");
	ExpectOutput("echo (2*(3+4))", "14");

	// commands that should fail
	ExpectMathError("echo (1/0)");
	ExpectMathError("echo (9223372036854775807+1)");
	ExpectSyntaxError("echo \"a\" | nosuch");
	ExpectSyntaxError("echo \"a\" \"b\"");

	std::cout << passed << " passed, " << failed << " failed\n";

	if (failed > 0) return 1;
	return 0;
}
