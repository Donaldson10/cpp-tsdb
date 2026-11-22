#include "tsdb/wal.hpp"
#include <stdexcept>
#include <string>
#include <array>
#include <bit>
#include <cstddef>

namespace tsdb {

    WAL::WAL(const std::filesystem::path& path){
        path_ = path;
        out_.open(path, std::ios::binary | std::ios::app);
        if(!out_){
            throw std::runtime_error("Couldn't open file:" + path.string());
        }
    }

    void WAL::append(const Point& point){
        
        out_.write(reinterpret_cast<const char*>(&point.timestamp), sizeof(point.timestamp));
        out_.write(reinterpret_cast<const char*>(&point.value), sizeof(point.value));
        out_.flush();
    }

    std::vector<Point> WAL::replay() const{
        std::vector<Point> points;
        std::ifstream file(path_, std::ios::binary);
        if(!file){
            return points;
        }
        Timestamp timestamp;
        double value;
        while (file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp)) &&
                file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
                    points.push_back({timestamp, value});
        }
        return points;
    }


}  // namespace tsdb
