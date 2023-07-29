#include <gtest/gtest.h>

#include "arg_parser/arg_parser.hpp"
#include "arg_parser/option.hpp"

TEST(ObjectTest, CheckValue) {
	// Test int value
	Object intObject(42);
	EXPECT_TRUE(intObject.isInt());
	EXPECT_EQ(intObject.getInt(), 42);

	// Test bool value
	Object boolObject(true);
	EXPECT_TRUE(boolObject.isBool());
	EXPECT_TRUE(boolObject.getBool());

	// Test string value
	Object stringObject(std::string("Hello, world!"));
	EXPECT_TRUE(stringObject.isString());
	EXPECT_EQ(stringObject.getString(), "Hello, world!");

	// Test list value
	Object listObject({ 1, std::string("two"), true });
	EXPECT_TRUE(listObject.isList());
	EXPECT_EQ(listObject.getList().size(), 3);
	EXPECT_EQ(listObject.getList()[0].getInt(), 1);
	EXPECT_EQ(listObject.getList()[1].getString(), "two");
	EXPECT_TRUE(listObject.getList()[2].getBool());
}

TEST(OptionTest, CheckOptionProperties) {
	std::vector<std::string> allowedValues = { "DEBUG", "INFO", "WARNING", "ERROR" };

	// Create an Option using newOption function
	Option option = newOption("verbose", "v", "Print verbose output.", "LEVEL",
														allowedValues, std::nullopt,
														Object("INFO"), std::nullopt,
														OptionType::single, std::nullopt, false, false);

	// Check properties of the option
	EXPECT_EQ(option.isFlag(), false);
	EXPECT_EQ(option.isSingle(), true);
	EXPECT_EQ(option.isMultiple(), false);

	EXPECT_EQ(option.valueOrDefault(Object("DEBUG")).getString(), "INFO");
	EXPECT_EQ(option.valueOrDefault(Object(42)).getInt(), 42);
	EXPECT_EQ(option.valueOrDefault(Object(true)).getBool(), true);

	// Check the aliases (abbreviations) of the option
	EXPECT_EQ(option.getAbbr().value(), "v");
	EXPECT_EQ(option.getAliases().value().size(), 0); // Replace 0 with the actual number of aliases

	// Check the mandatory and hide properties of the option
	EXPECT_EQ(option.isMandatory(), false);
	EXPECT_EQ(option.isHidden(), false);
}

// More tests on Option validation
TEST(OptionTest, CheckOptionValidation) {
	// Test empty name validation
	EXPECT_THROW(newOption(""), std::invalid_argument);

	// Test name starting with '-' validation
	EXPECT_THROW(newOption("-invalid"), std::invalid_argument);

	// Test invalid character in name validation
	EXPECT_THROW(newOption("invalid\t"), std::invalid_argument);

	// Test invalid abbreviation
	EXPECT_THROW(newOption("verbose", "-v"), std::invalid_argument);

	// Test invalid abbreviation with invalid character
	EXPECT_THROW(newOption("verbose", "v!@#"), std::invalid_argument);
}

// Additional tests for Option class if you have custom callbacks
TEST(OptionTest, CheckOptionCallback) {
	bool callbackCalled = false;

	// Define a callback function
	auto callbackFunc = [&](const Object& value) {
			callbackCalled = true;
			// Add any assertions or checks you want to perform in the callback
	};

	// Create an Option with a callback
	Option option = newOption("output", std::nullopt, std::nullopt, "FILENAME",
														std::nullopt, std::nullopt, std::nullopt,
														callbackFunc, OptionType::single, std::nullopt,
														std::nullopt);

	// Invoke the callback
	Object object = Object(std::string("output.txt"));
	(option.getCallback().value())(object);

	// Check if the callback was called
	EXPECT_TRUE(callbackCalled);
}


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}