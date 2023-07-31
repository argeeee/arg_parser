#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

/// An exception thrown by `ArgParser`.
class ArgParserException : public std::runtime_error {
 public:
	/// The command(s) that were parsed before discovering the error.
	///
	/// This will be empty if the error was on the root parser.
	std::vector<std::string> commands;

	ArgParserException(const std::string& message, const std::vector<std::string>& commands = {})
		: std::runtime_error(message), commands(commands)
	{
	}
};
