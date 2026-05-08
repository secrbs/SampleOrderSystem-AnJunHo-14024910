#pragma once
#include <string>
#include <vector>
#include <optional>
#include "../model/Sample.h"
#include "../repository/SampleRepository.h"

class SampleController {
public:
    explicit SampleController(SampleRepository& repo);

    bool registerSample(const Sample& sample);
    std::vector<Sample> getAll() const;
    std::vector<Sample> searchByName(const std::string& keyword) const;
    std::optional<Sample> findById(const std::string& id) const;

private:
    SampleRepository& repo_;
    bool validate(const Sample& s) const;
};
