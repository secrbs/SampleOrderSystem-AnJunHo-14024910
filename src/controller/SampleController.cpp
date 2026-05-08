#include "SampleController.h"
#include <algorithm>
#include <stdexcept>

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
    std::vector<Sample> result;
    for (const auto& s : repo_.findAll()) {
        if (s.name.find(keyword) != std::string::npos)
            result.push_back(s);
    }
    return result;
}

std::optional<Sample> SampleController::findById(const std::string& id) const {
    return repo_.findById(id);
}

bool SampleController::validate(const Sample& sample) const {
    return sample.yield > 0.0 && sample.yield <= 1.0;
}
