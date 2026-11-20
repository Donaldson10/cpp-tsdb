#pragma once          

#include <cstdint>    // for int64_t
#include <vector>     // for std::vector

namespace tsdb {

using Timestamp = std::int64_t;

struct Point {
    Timestamp timestamp;
    double value;   
};

class Series {
    public: 
        void insert(Timestamp timestamp, double value);
        std::vector<Point> query(Timestamp start, Timestamp end) const;
        std::size_t size() const; 

    private: 
        std::vector<Point> points_;
};




}  // namespace tsdb