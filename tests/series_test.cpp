#include <gtest/gtest.h>
#include "tsdb/series.hpp"

TEST(SeriesTest, EmptySeriesReturnsNothing) {
    tsdb::Series s;
    auto result = s.query(0, 1000);
    EXPECT_TRUE(result.empty());
}
TEST(SeriesTest, SeriesReturnsCorrectPoints) {
    tsdb::Series s; 
    s.insert(3000, 23.0);
    s.insert(1000, 21.0);   // exactly at start: should be included
    s.insert(5000, 25.0);   // after the range: excluded
    s.insert(2000, 22.0);
    s.insert(500,  20.5);   // before the range: excluded
    s.insert(4000, 24.0);   // exactly at end: excluded (half-open)

    EXPECT_EQ(s.size(), 6u);

    auto result = s.query(1000, 4000);

    ASSERT_EQ(result.size(), 3u);

    EXPECT_EQ(result[0].value, 21.0);
    EXPECT_EQ(result[1].value, 22.0);
    EXPECT_EQ(result[2].value, 23.0);

    EXPECT_EQ(result[0].timestamp, 1000);
    EXPECT_EQ(result[1].timestamp, 2000);
    EXPECT_EQ(result[2].timestamp, 3000);
}
TEST(SeriesTest, SeriesDuplicateOverwritesCorrectly){
    tsdb::Series s; 
    s.insert(2000, 27.0); // insert first data point
    s.insert(2000, 28.5); // insert at same time to check overwrite
    s.insert(1000, 24.0);
    s.insert(3000, 31.0);
    s.insert(500, 22.5);
    s.insert(4000, 35.0);
    s.insert(3000, 31.5);

    EXPECT_EQ(s.size(), 5u);

    auto result = s.query(1000, 4000);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[1].value, 28.5);
    EXPECT_EQ(result[2].value, 31.5);
}
TEST(SeriesTest, SeriesStartAfterEnd){
    tsdb::Series s; 
    s.insert(2000, 27.0);
    s.insert(1000, 24.0);
    s.insert(3000, 31.0);
    s.insert(500, 22.5);
    s.insert(4000, 35.0);
    EXPECT_EQ(s.size(), 5u);
    auto result = s.query(4000, 1000);
    EXPECT_TRUE(result.empty());
}