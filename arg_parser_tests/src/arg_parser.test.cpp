#include <gtest/gtest.h>

#include "arg_parser/arg_parser.hpp"

// Test the constructor and getter functions
TEST(OptionTest, ConstructorAndGetter) {
	Option option("name", 'a', true, {"value1", "value2"});
	
	EXPECT_EQ(option.getName(), "name");
	EXPECT_EQ(option.getAbbr(), 'a');
	EXPECT_TRUE(option.isMandatory());
	EXPECT_EQ(option.getAllowedValues(), (std::vector<std::string>{"value1", "value2"}));
}

// Test adding allowed values
TEST(OptionTest, AddAllowedValue) {
	Option option("name");
	option.addAllowedValue("value1");
	option.addAllowedValue("value2");
	
	EXPECT_EQ(option.getAllowedValues(), (std::vector<std::string>{"value1", "value2"}));
}

// Test setting a default value
TEST(OptionTest, SetDefaultValue) {
	Option option("name");
	option.setDefaultValue("default_value");
	
	EXPECT_EQ(option.getDefaultValue(), "default_value");
}

// Test setting a callback function
TEST(OptionTest, SetCallback) {
	Option option("name");
	bool callbackCalled = false;
	
	option.setCallback([&callbackCalled](const std::string& value) {
		callbackCalled = true;
		EXPECT_EQ(value, "some_value"); // Check if the correct value is passed to the callback
	});
	
	const auto& callbackFunction = option.getCallback();
	ASSERT_TRUE(callbackFunction); // Ensure that the callback is set

	// Simulate invoking the callback
	if (callbackFunction) {
		callbackFunction("some_value");
	}

	EXPECT_TRUE(callbackCalled);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}