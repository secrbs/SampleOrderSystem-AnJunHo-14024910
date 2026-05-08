#include "SampleController.h"
#include <algorithm>
#include <cctype>

SampleController::SampleController(SampleRepository& repo) : repo_(repo) {}

bool SampleController::registerSample(const Sample& sample) {
    if (!validate(sample)) return false;
    if (repo_.findById(sample.id).has_value()) return false;
    repo_.save(sample);
    return true;
}

std::vector<Sample> SampleController::getAll() const {
    return repo_.findAll();
}

std::vector<Sample> SampleController::searchByName(const std::string& keyword) const {
    auto toLower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return s;
    };
    std::string kw = toLower(keyword);
    std::vector<Sample> result;
    for (const auto& s : repo_.findAll()) {
        // ID, 이름 중 어디든 keyword가 포함되면 검색
        if (toLower(s.id).find(kw)   != std::string::npos ||
            toLower(s.name).find(kw) != std::string::npos)
            result.push_back(s);
    }
    return result;
}

std::optional<Sample> SampleController::findById(const std::string& id) const {
    return repo_.findById(id);
}

bool SampleController::validate(const Sample& s) const {
    return s.yield > 0.0 && s.yield <= 1.0;
}
