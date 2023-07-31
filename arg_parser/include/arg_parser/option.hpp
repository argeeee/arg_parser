#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <regex>
#include <optional>

enum class OptionType {
	flag,
	single,
	multiple
};

// Forward declaration of the Option class
class Option;

// Class representing an Object that can be a number, boolean, or string
class Object {
 public:
	using ValueType = std::variant<int, bool, std::string, std::vector<Object>>;

	Object() : value() {}
	Object(int val) : value(val) {}
	Object(bool val) : value(val) {}
	Object(const std::string& val) : value(val) {}
	Object(const std::vector<Object>& val) : value(val) {}

	// Getter functions for the Object's value
	bool isInt() const { return std::holds_alternative<int>(value); }
	bool isBool() const { return std::holds_alternative<bool>(value); }
	bool isString() const { return std::holds_alternative<std::string>(value); }
	bool isList() const { return std::holds_alternative<std::vector<Object>>(value); }

	int getInt() const { return std::get<int>(value); }
	bool getBool() const { return std::get<bool>(value); }
	const std::string& getString() const { return std::get<std::string>(value); }
	const std::vector<Object>& getList() const { return std::get<std::vector<Object>>(value); }

	std::string toString() const {
		std::stringstream ss;
		std::visit([&ss](const auto& arg) {
				ss << arg;
		}, value);
		return ss.str();
	}

private:
	ValueType value;
};

// Overload the << operator for std::vector<Object>
std::ostream& operator<<(std::ostream& os, const std::vector<Object>& vec) {
	os << "[";
	for (size_t i = 0; i < vec.size(); ++i) {
		os << vec[i].toString();
		if (i < vec.size() - 1)
			os << ", ";
	}
	os << "]";
	return os;
}

// Function to create a new Option
Option newOption(
	const std::string& name,
	const std::optional<std::string>& abbr = std::nullopt,
	const std::optional<std::string>& help = std::nullopt,
	const std::optional<std::string>& valueHelp = std::nullopt,
	const std::optional<std::vector<std::string>>& allowed = std::nullopt,
	const std::optional<std::map<std::string, std::string>>& allowedHelp = std::nullopt,
	const std::optional<Object>& defaultsTo = std::nullopt,
	const std::optional<std::function<void(const Object&)>>& callback = std::nullopt,
	OptionType type = OptionType::single,
	const std::optional<bool>& negatable = std::nullopt,
	const std::optional<bool>& splitCommas = std::nullopt,
	bool mandatory = false,
	bool hide = false,
	const std::optional<std::vector<std::string>>& aliases = std::nullopt
);

// Class representing a command-line option
class Option {
 public:
	// Constructor is private to enforce use of newOption function
	Option(
		const std::string& name,
		const std::optional<std::string>& abbr,
		const std::optional<std::string>& help,
		const std::optional<std::string>& valueHelp,
		const std::optional<std::vector<std::string>>& allowed,
		const std::optional<std::map<std::string, std::string>>& allowedHelp,
		const std::optional<Object>& defaultsTo,
		const std::optional<std::function<void(const Object&)>>& callback,
		OptionType type,
		const std::optional<bool>& negatable,
		const std::optional<bool>& splitCommas,
		bool mandatory,
		bool hide,
		const std::optional<std::vector<std::string>>& aliases
	) : name(name), abbr(abbr), help(help), valueHelp(valueHelp),
			allowed(allowed), allowedHelp(allowedHelp), defaultsTo(defaultsTo),
			callback(callback), type(type), negatable(negatable),
			splitCommas(splitCommas), mandatory(mandatory), hide(hide),
			aliases(aliases)
	{
		if (name.empty()) {
			throw std::invalid_argument("Name cannot be empty.");
		} else if (name[0] == '-') {
			throw std::invalid_argument("Name cannot start with \"-\".");
		}

		// Ensure name does not contain any invalid characters.
		const std::regex _invalidChars("[ \t\r\n\"\\\\/']");
		if (std::regex_search(name, _invalidChars)) {
			throw std::invalid_argument("Name \"" + name + "\" contains invalid characters.");
		}

		if (abbr.has_value()) {
			const std::string& abbrValue = abbr.value();
			if (abbrValue.length() != 1) {
				throw std::invalid_argument("Abbreviation must be null or have length 1.");
			} else if (abbrValue[0] == '-') {
				throw std::invalid_argument("Abbreviation cannot be \"-\".");
			}

			if (std::regex_search(abbrValue, _invalidChars)) {
				throw std::invalid_argument("Abbreviation is an invalid character.");
			}
		}
	}

	// Getters for option type
	bool isFlag() const { return type == OptionType::flag; }
	bool isSingle() const { return type == OptionType::single; }
	bool isMultiple() const { return type == OptionType::multiple; }

	const std::string& getName() const { return name; }
	const std::optional<std::string>& getHelp() const { return help; }
	const std::optional<std::string>& getValueHelp() const { return valueHelp; }
	const std::optional<std::vector<std::string>>& getAllowed() const { return allowed; }
	const std::optional<std::map<std::string, std::string>>& getAllowedHelp() const { return allowedHelp; }
	const std::optional<Object>& getDefaultsTo() const { return defaultsTo; }
	const std::optional<std::function<void(const Object&)>>& getCallback() const { return callback; }
	OptionType getType() const { return type; }
	const std::optional<bool>& isNegatable() const { return negatable; }
	const std::optional<bool>& shouldSplitCommas() const { return splitCommas; }
	const std::optional<std::string>& getAbbr() const { return abbr; }
	const std::optional<std::vector<std::string>>& getAliases() const { return aliases; }

	bool isMandatory() const { return mandatory; }
	bool isHidden() const { return hide; }

	// Method to return the value if non-null, otherwise the default value
	Object valueOrDefault(const Object& value) const {
		if (value.isBool() || value.isInt() || !value.getString().empty()) return value;
		if (isMultiple()) return defaultsTo.value_or(Object(std::vector<Object>()));
		return defaultsTo.value_or(Object());
	}

	// Deprecated method (use valueOrDefault instead)
	Object getOrDefault(const Object& value) const {
		return valueOrDefault(value);
	}

 private:
	std::string name;
	std::optional<std::string> abbr;
	std::optional<std::string> help;
	std::optional<std::string> valueHelp;
	std::optional<std::vector<std::string>> allowed;
	std::optional<std::map<std::string, std::string>> allowedHelp;
	std::optional<Object> defaultsTo;
	std::optional<std::function<void(const Object&)>> callback;
	OptionType type;
	std::optional<bool> negatable;
	std::optional<bool> splitCommas;
	bool mandatory;
	bool hide;
	std::optional<std::vector<std::string>> aliases;
};

// Implementation of the newOption function
Option newOption(
    const std::string& name,
    const std::optional<std::string>& abbr,
    const std::optional<std::string>& help,
    const std::optional<std::string>& valueHelp,
    const std::optional<std::vector<std::string>>& allowed,
    const std::optional<std::map<std::string, std::string>>& allowedHelp,
    const std::optional<Object>& defaultsTo,
    const std::optional<std::function<void(const Object&)>>& callback,
    OptionType type,
    const std::optional<bool>& negatable,
    const std::optional<bool>& splitCommas,
    bool mandatory,
    bool hide,
    const std::optional<std::vector<std::string>>& aliases
) {
    return Option(name, abbr, help, valueHelp, allowed, allowedHelp, defaultsTo,
                  callback, type, negatable, splitCommas, mandatory, hide, aliases);
}
