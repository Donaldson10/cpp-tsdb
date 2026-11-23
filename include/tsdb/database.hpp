#pragma once
#include <tsdb/wal.hpp>

namespace tsdb {
    class Database{
        public:
            explicit Database(const std::filesystem::path &path);
            void insert(Timestamp timestamp, double value);
            std::vector<Point> query(Timestamp start, Timestamp end) const;
            std::size_t size() const; 
        private:
            WAL log_;
            Series series_;

    };
} // namespace tsdb