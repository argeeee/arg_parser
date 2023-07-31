#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <queue>
#include <variant>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>

#include "arg_parser/option.hpp"

// Forward declaration of wrapTextAsLines function
std::vector<std::string> wrapTextAsLines(const std::string& text, int start, int length);

class _Usage {
public:
	static const int _columnCount = 3;

	_Usage(const std::vector<std::variant<Option, std::string>>& optionsAndSeparators, int lineLength)
		: _optionsAndSeparators(optionsAndSeparators), lineLength(lineLength) {
		_columnWidths = _calculateColumnWidths();
	}

	std::string generate() {
		for (const auto& optionOrSeparator : _optionsAndSeparators) {
			if (std::holds_alternative<std::string>(optionOrSeparator)) {
				_writeSeparator(std::get<std::string>(optionOrSeparator));
			} else {
				const Option& option = std::get<Option>(optionOrSeparator);
				if (!option.isHidden()) {
					_writeOption(option);
				}
			}
		}

		return _buffer.str();
	}

private:
	const std::vector<std::variant<Option, std::string>>& _optionsAndSeparators;
	std::ostringstream _buffer;
	int _currentColumn = 0;
	std::vector<int> _columnWidths;
	int _newlinesNeeded = 0;
	int lineLength;

	void _writeSeparator(const std::string& separator) {
		if (!_buffer.str().empty()) {
			_buffer << "\n\n";
		}
		_buffer << separator;
		_newlinesNeeded = 1;
	}

	void _writeOption(const Option& option) {
		_write(0, _abbreviation(option));
		_write(1, _longOption(option) + _mandatoryOption(option));

		if (option.getHelp().has_value() && !option.getHelp().value().empty()) {
			_write(2, option.getHelp().value());
		}

		if (option.getAllowedHelp().has_value()) {
			std::vector<std::string> allowedNames;
			for (const auto& entry : option.getAllowedHelp().value()) {
				allowedNames.push_back(entry.first);
			}
			std::sort(allowedNames.begin(), allowedNames.end());
			_newline();
			for (const auto& name : allowedNames) {
				_write(1, _allowedTitle(option, name));
				_write(2, option.getAllowedHelp().value().at(name));
			}
			_newline();
		} else if (option.getAllowed().has_value()) {
			_write(2, _buildAllowedList(option));
		} else if (option.isFlag()) {
			if (option.getDefaultsTo().value().getBool() == true) {
				_write(2, "(defaults to on)");
			}
		} else if (option.isMultiple()) {
			if (option.getDefaultsTo().has_value() && !option.getDefaultsTo().value().getList().empty()) {
				std::ostringstream defaults;
				for (const auto& value : option.getDefaultsTo().value().getList()) {
					if (defaults.tellp() > 0) {
						defaults << ", ";
					}
					defaults << "\"" << value.toString() << "\"";
				}
				_write(2, "(defaults to " + defaults.str() + ")");
			}
		} else if (option.getDefaultsTo().has_value()) {
			_write(2, "(defaults to \"" + option.getDefaultsTo().value().toString() + "\")");
		}
	}

	std::string _abbreviation(const Option& option) {
		return option.getAbbr().value().empty() ? "" : "-" + option.getAbbr().value() + ", ";
	}

	std::string _longOption(const Option& option) {
		std::string result;
		if (option.isNegatable()) {
			result = "--[no-]" + option.getName();
		} else {
			result = "--" + option.getName();
		}

		if (!option.getValueHelp().value().empty()) {
			result += "=<" + option.getValueHelp().value() + ">";
		}

		return result;
	}

	std::string _mandatoryOption(const Option& option) {
		return option.isMandatory() ? " (mandatory)" : "";
	}

	std::string _allowedTitle(const Option& option, const std::string& allowed) {
			bool isDefault = false;
		if (option.getDefaultsTo().has_value()) {
			if (option.getDefaultsTo().value().isList()) {
				isDefault = std::find(
					option.getDefaultsTo().value().getList().begin(),
					option.getDefaultsTo().value().getList().end(),
				allowed) != option.getDefaultsTo().value().getList().end();
			} else {
				isDefault = allowed == option.getDefaultsTo().value().toString();
			}
		}
		return "      [" + allowed + "]" + (isDefault ? " (default)" : "");
	}

	std::vector<int> _calculateColumnWidths() {
		int abbr = 0;
		int title = 0;
		for (const auto& optionOrSeparator : _optionsAndSeparators) {
			if (std::holds_alternative<Option>(optionOrSeparator)) {
				const Option& option = std::get<Option>(optionOrSeparator);
				if (!option.isHidden()) {
					abbr = std::max(abbr, static_cast<int>(_abbreviation(option).length()));
					title = std::max(title, static_cast<int>(_longOption(option).length() + _mandatoryOption(option).length()));
					if (option.getAllowedHelp().has_value()) {
						for (const auto& entry : option.getAllowedHelp().value()) {
							title = std::max(title, static_cast<int>(_allowedTitle(option, entry.first).length()));
						}
					}
				}
			}
		}
		title += 4;
		return { abbr, title };
	}

	void _newline() {
		_newlinesNeeded++;
		_currentColumn = 0;
	}

	void _write(int column, const std::string& text) {
		std::vector<std::string> lines = { text };
		if (column == _columnWidths.size() && lineLength > 0) {
			int start = std::accumulate(_columnWidths.begin(), _columnWidths.begin() + column, 0);
			lines = {};
			for (const auto& line : wrapTextAsLines(text, start, lineLength)) {
				lines.push_back(line);
			}
		}

		while (!lines.empty() && lines.front().empty()) {
			lines.erase(lines.begin());
		}
		while (!lines.empty() && lines.back().empty()) {
			lines.pop_back();
		}

		for (const auto& line : lines) {
			_writeLine(column, line);
		}
	}

	void _writeLine(int column, const std::string& text) {
		while (_newlinesNeeded > 0) {
			_buffer << '\n';
			_newlinesNeeded--;
		}

		while (_currentColumn != column) {
			if (_currentColumn < _columnCount - 1) {
				_buffer << std::string(_columnWidths[_currentColumn], ' ');
			} else {
				_buffer << '\n';
			}
			_currentColumn = (_currentColumn + 1) % _columnCount;
		}

		if (column < _columnWidths.size()) {
			_buffer << text << std::string(_columnWidths[column] - text.length(), ' ');
		} else {
			_buffer << text;
		}

		_currentColumn = (_currentColumn + 1) % _columnCount;
		if (column == _columnCount - 1) {
			_newlinesNeeded++;
		}
	}

	std::string _buildAllowedList(const Option& option) {
		auto isDefault = [&option](const std::string& value) {
			if (option.getDefaultsTo().has_value()) {
				if (option.getDefaultsTo().value().isList()) {
					return std::find(
						option.getDefaultsTo().value().getList().begin(),
						option.getDefaultsTo().value().getList().end(),
						value) != option.getDefaultsTo().value().getList().end();
				} else {
					return value == option.getDefaultsTo().value().toString();
				}
			}
			return false;
		};

		std::ostringstream allowedBuffer;
		allowedBuffer << "[";
		bool first = true;
		for (const auto& allowed : option.getAllowed().value()) {
			if (!first) {
				allowedBuffer << ", ";
			}
			allowedBuffer << allowed;
			if (isDefault(allowed)) {
				allowedBuffer << " (default)";
			}
			first = false;
		}
		allowedBuffer << "]";
		return allowedBuffer.str();
	}
};

std::string generateUsage(const std::vector<std::variant<Option, std::string>>& optionsAndSeparators, int lineLength) {
	_Usage usage(optionsAndSeparators, lineLength);
	return usage.generate();
}
