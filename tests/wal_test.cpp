#include <gtest/gtest.h>
#include <filesystem>

#include "tsdb/wal.hpp"

TEST(WalTest, ReplayReturnsAppendedPoints) {
    // 1. Setup: a clean temp file just for this test
    auto path = std::filesystem::temp_directory_path() / "wal_roundtrip.wal";
    std::filesystem::remove(path);   // delete leftovers from earlier runs

    // 2. Write phase: the WAL only lives inside these braces
    {
        tsdb::WAL wal(path);
        wal.append({1000,24.5});       // a few points, try one out of order
        wal.append({4500, 29.0});
        wal.append({3000, 27.0});
        wal.append({4000, 28.5});
        
    }   // <- wal is destroyed here: file closed, like the program exiting

    // 3. Read phase: a brand-new WAL on the same file
    tsdb::WAL wal2(path);
    auto result = wal2.replay();

    // 4. Checks
    ASSERT_EQ(result.size(), 4u);  // size first

    EXPECT_EQ(result[0].value, 24.5);
    EXPECT_EQ(result[1].value, 29.0);
    EXPECT_EQ(result[2].value, 27.0);
    EXPECT_EQ(result[3].value, 28.5);
    

    EXPECT_EQ(result[0].timestamp, 1000);
    EXPECT_EQ(result[1].timestamp, 4500);
    EXPECT_EQ(result[2].timestamp, 3000);
    EXPECT_EQ(result[3].timestamp, 4000);  // timestamp and value of each point, in append order
}
