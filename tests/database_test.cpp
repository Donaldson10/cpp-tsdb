#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "tsdb/database.hpp"

TEST(DatabaseTest, DataSurvivesRestart){
    // 1. Setup: a clean temp file just for this test
    auto path = std::filesystem::temp_directory_path() / "database_setup.wal";
    std::filesystem::remove(path);   // delete leftovers from earlier runs

    
    {
        tsdb::Database db(path);
        db.insert(1000, 25.0);
        db.insert(1500, 27.5);
        db.insert(4500, 35.5);
        db.insert(2000, 29.0);
        db.insert(4000, 31.5);
        db.insert(1500, 28.0);

        ASSERT_EQ(db.size(), 5u);

        std::vector<tsdb::Point> answer = db.query(1000, 4500);
        ASSERT_EQ(answer.size(), 4u);

        EXPECT_EQ(answer[0].timestamp, 1000);
        EXPECT_EQ(answer[1].timestamp, 1500);
        EXPECT_EQ(answer[2].timestamp, 2000);
        EXPECT_EQ(answer[3].timestamp, 4000);

        EXPECT_EQ(answer[0].value,  25.0);
        EXPECT_EQ(answer[1].value,  28.0);
        EXPECT_EQ(answer[2].value,  29.0);
        EXPECT_EQ(answer[3].value,  31.5);

    }

    tsdb::Database db2(path);
    ASSERT_EQ(db2.size(), 5u);

    std::vector<tsdb::Point> answer = db2.query(1000, 4500);
    ASSERT_EQ(answer.size(), 4u);

    EXPECT_EQ(answer[0].timestamp, 1000);
    EXPECT_EQ(answer[1].timestamp, 1500);
    EXPECT_EQ(answer[2].timestamp, 2000);
    EXPECT_EQ(answer[3].timestamp, 4000);

    EXPECT_EQ(answer[0].value,  25.0);
    EXPECT_EQ(answer[1].value,  28.0);
    EXPECT_EQ(answer[2].value,  29.0);
    EXPECT_EQ(answer[3].value,  31.5);
}

TEST(DatabaseTest, RestartsDoNotGrowWal){
    auto path = std::filesystem::temp_directory_path() / "restart_grow.wal";
    std::filesystem::remove(path);   // delete leftovers from earlier runs
    {
        tsdb::Database db(path);
        db.insert(1000, 25.0);
        db.insert(1500, 27.5);
        db.insert(4500, 35.5);
        db.insert(2000, 29.0);
        db.insert(4000, 31.5);
        db.insert(1500, 28.0);

        ASSERT_EQ(db.size(), 5u);
    }

    ASSERT_EQ(std::filesystem::file_size(path), 6u * sizeof(tsdb::Point));

    {
        tsdb::Database db(path);
        ASSERT_EQ(db.size(), 5u);
    }

    ASSERT_EQ(std::filesystem::file_size(path), 6u * sizeof(tsdb::Point));

}

TEST(DatabaseTest, CrashRecoveryTest){
    // 1. Setup: a clean temp file just for this test
    auto path = std::filesystem::temp_directory_path() / "crash_recovery.wal";
    std::filesystem::remove(path);   // delete leftovers from earlier runs

    
    {
        tsdb::Database db(path);
        db.insert(1000, 25.0);
        db.insert(1500, 27.5);
        db.insert(4500, 35.5);
        db.insert(2000, 29.0);
        db.insert(4000, 31.5);
        db.insert(1500, 28.0);

        ASSERT_EQ(db.size(), 5u);
    }

    {
        std::ofstream out(path, std::ios::binary | std::ios::app);
        ASSERT_TRUE(out);

        tsdb::Point p = {3500, 28.0};
        
        out.write(reinterpret_cast<const char*>(&p.timestamp), sizeof(p.timestamp));
        out.flush();
    }
    
    tsdb::Database db2(path);
    ASSERT_EQ(db2.size(), 5u);

    std::vector<tsdb::Point> answer = db2.query(1000, 4500);
    ASSERT_EQ(answer.size(), 4u);

    EXPECT_EQ(answer[0].timestamp, 1000);
    EXPECT_EQ(answer[1].timestamp, 1500);
    EXPECT_EQ(answer[2].timestamp, 2000);
    EXPECT_EQ(answer[3].timestamp, 4000);

    EXPECT_EQ(answer[0].value,  25.0);
    EXPECT_EQ(answer[1].value,  28.0);
    EXPECT_EQ(answer[2].value,  29.0);
    EXPECT_EQ(answer[3].value,  31.5);

    ASSERT_EQ(std::filesystem::file_size(path), 6u * sizeof(tsdb::Point) + 8);

}