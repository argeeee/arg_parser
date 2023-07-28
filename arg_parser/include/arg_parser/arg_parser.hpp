#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

class Option {
 public:
	Option(const std::string& name, char abbr = '\0', bool mandatory = false, std::vector<std::string> allowedValues = {})
		: name(name), abbr(abbr), mandatory(mandatory), allowedValues(allowedValues)
	{
	}

	// Add allowed value
	void addAllowedValue(const std::string& value) {
		allowedValues.push_back(value);
	}

	// Set default value
	void setDefaultValue(const std::string& value) {
		defaultValue = value;
	}

	// Set callback function
	void setCallback(std::function<void(const std::string&)> callback) {
		this->callback = std::move(callback);
	}

	// Getter functions
	const std::string& getName() const {
		return name;
	}

	char getAbbr() const {
		return abbr;
	}

	bool isMandatory() const {
		return mandatory;
	}

	const std::vector<std::string>& getAllowedValues() const {
		return allowedValues;
	}

	const std::string& getDefaultValue() const {
		return defaultValue;
	}

	const std::function<void(const std::string&)>& getCallback() const {
		return callback;
	}

 private:
	std::string name;
	char abbr;
	bool mandatory;
	std::vector<std::string> allowedValues;
	std::string defaultValue;
	std::function<void(const std::string&)> callback;
};

class Flag {
public:
	Flag(const std::string& name, char abbr = '\0', bool negatable = true)
		: name(name), abbr(abbr), negatable(negatable)
	{
	}

	// Set callback function
	void setCallback(std::function<void()> callback) {
		this->callback = std::move(callback);
	}

	// Getters
	const std::string& getName() const {
		return name;
	}

	char getAbbr() const {
		return abbr;
	}

	bool isNegatable() const {
		return negatable;
	}

	const std::function<void()>& getCallback() const {
		return callback;
	}

private:
	std::string name;
	char abbr;
	bool negatable;
	std::function<void()> callback;
};
