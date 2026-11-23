#include <tsdb/database.hpp>
#include <filesystem>

namespace tsdb{
     Database::Database(const std::filesystem::path &path) : log_(path) {
        std::vector<Point> save = log_.replay();
        for (const auto& p : save){
            series_.insert(p.timestamp, p.value);
        }
    }
    void Database::insert(Timestamp timestamp, double value){
        log_.append({timestamp,value});
        series_.insert(timestamp, value);
    }
    std::vector<Point> Database::query(Timestamp start, Timestamp end) const{
        return series_.query(start, end);
    }
    std::size_t Database::size() const{
        return series_.size();

    }
} // namespace tsdb