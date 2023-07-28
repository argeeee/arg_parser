#include <gtest/gtest.h>

#include "arg_parser/arg_parser.hpp"

// Example test case
TEST(ArgParserTest, testMethod) {
	ASSERT_EQ(test(), 42);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}