#pragma once
#include <string>
#include <vector>
#include <optional>
#include "../model/Sample.h"

class SampleRepository {
public:
    explicit SampleRepository(const std::string& filePath);

    std::vector<Sample> findAll() const;
    std::optional<Sample> findById(const std::string& id) const;
    int count() const;
    int totalStock() const;

    void save(const Sample& sample);
    bool update(const Sample& sample);

private:
    std::string filePath_;
    std::vector<Sample> samples_;

    void load();
    void persist() const;
};
