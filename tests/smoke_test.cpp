#include <gtest/gtest.h>

#include "tsdb/version.hpp"

// Proves the test setup works. Replace with real tests as you build.
TEST(Smoke, VersionIsSet) { EXPECT_FALSE(tsdb::version().empty()); }
