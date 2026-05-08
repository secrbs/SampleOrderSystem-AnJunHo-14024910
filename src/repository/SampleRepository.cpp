#include "SampleRepository.h"
#include <fstream>
#include <numeric>
#include <algorithm>
#include <stdexcept>

SampleRepository::SampleRepository(const std::string& filePath)
    : filePath_(filePath) { load(); }

std::vector<Sample> SampleRepository::findAll() const { return items_; }

std::optional<Sample> SampleRepository::findById(const std::string& id) const {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&id](const Sample& s){ return s.id == id; });
    return it == items_.end() ? std::nullopt : std::optional<Sample>(*it);
}

int SampleRepository::count() const { return static_cast<int>(items_.size()); }

int SampleRepository::totalStock() const {
    return std::accumulate(items_.begin(), items_.end(), 0,
        [](int s, const Sample& x){ return s + x.stock; });
}

void SampleRepository::save(const Sample& s) {
    if (findById(s.id)) throw std::runtime_error("Duplicate ID: " + s.id);
    items_.push_back(s);
    persist();
}

bool SampleRepository::update(const Sample& s) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&s](const Sample& x){ return x.id == s.id; });
    if (it == items_.end()) return false;
    *it = s; persist(); return true;
}

void SampleRepository::load() {
    std::ifstream f(filePath_);
    if (!f.is_open()) { items_.clear(); return; }
    try { nlohmann::json j; f >> j; items_ = j.get<std::vector<Sample>>(); }
    catch (...) { items_.clear(); }
}

void SampleRepository::persist() const {
    std::ofstream f(filePath_);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + filePath_);
    f << nlohmann::json(items_).dump(2);
}
