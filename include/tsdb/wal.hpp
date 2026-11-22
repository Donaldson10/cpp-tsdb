#pragma once

#include "tsdb/series.hpp"

#include <fstream>
#include <string>
#include <filesystem>

namespace tsdb{
    class WAL{
        public: 
            explicit WAL(const std::filesystem::path& path);
            void append(const Point& point);
            std::vector<Point> replay() const;
            
        private:
            std::ofstream out_;
            std::filesystem::path path_;
    };
} // namespace tsdb