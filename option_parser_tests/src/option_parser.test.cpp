#include <gtest/gtest.h>

#include "option_parser/option_parser.hpp"

// Example test case
TEST(OptionParserTest, test) {
	ASSERT_EQ(test(), 42);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}