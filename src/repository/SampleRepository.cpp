#include "SampleRepository.h"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <optional>
#include <stdexcept>

SampleRepository::SampleRepository(const std::string& filePath)
    : filePath_(filePath) {
    load();
}

std::vector<Sample> SampleRepository::findAll() const {
    return samples_;
}

std::optional<Sample> SampleRepository::findById(const std::string& id) const {
    auto it = std::find_if(samples_.begin(), samples_.end(),
        [&id](const Sample& s) { return s.id == id; });
    if (it == samples_.end()) return std::nullopt;
    return *it;
}

int SampleRepository::count() const {
    return static_cast<int>(samples_.size());
}

int SampleRepository::totalStock() const {
    return std::accumulate(samples_.begin(), samples_.end(), 0,
        [](int sum, const Sample& s) { return sum + s.stock; });
}

void SampleRepository::save(const Sample& sample) {
    auto it = std::find_if(samples_.begin(), samples_.end(),
        [&sample](const Sample& s) { return s.id == sample.id; });
    if (it != samples_.end())
        throw std::runtime_error("ID already exists: " + sample.id);
    samples_.push_back(sample);
    persist();
}

bool SampleRepository::update(const Sample& sample) {
    auto it = std::find_if(samples_.begin(), samples_.end(),
        [&sample](const Sample& s) { return s.id == sample.id; });
    if (it == samples_.end()) return false;
    *it = sample;
    persist();
    return true;
}

void SampleRepository::load() {
    std::ifstream f(filePath_);
    if (!f.is_open()) { samples_.clear(); return; }
    try {
        nlohmann::json j;
        f >> j;
        samples_ = j.get<std::vector<Sample>>();
    } catch (...) {
        samples_.clear();
    }
}

void SampleRepository::persist() const {
    std::ofstream f(filePath_);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + filePath_);
    f << nlohmann::json(samples_).dump(2);
}
