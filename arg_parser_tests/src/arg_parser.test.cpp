#include <gtest/gtest.h>

#include "arg_parser/arg_parser.hpp"


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}