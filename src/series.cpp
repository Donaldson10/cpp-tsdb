#include "tsdb/series.hpp"
#include <algorithm> 
namespace tsdb{

    namespace{
        auto before = [](const Point& p, Timestamp t){return p.timestamp < t;};
    }
    std::size_t Series::size() const{
        return points_.size();
    }

    std::vector<Point> Series::query(Timestamp start, Timestamp end) const{
        if (start > end) return {};
        
        auto first = std::lower_bound(points_.begin(), points_.end(), start, before);
        auto last = std::lower_bound(points_.begin(), points_.end(), end, before);
        return {first, last};
    }

    void Series::insert(Timestamp timestamp, double value){
        auto it = std::lower_bound(points_.begin(), points_.end(), timestamp, before);

        if (it != points_.end() && it->timestamp == timestamp) {
            it->value = value;
        } else {
            points_.insert(it, {timestamp, value});
        }

    }
}