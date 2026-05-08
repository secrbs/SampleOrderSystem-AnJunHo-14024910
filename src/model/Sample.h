#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Sample {
    std::string id;
    std::string name;
    double avgProductionTime;
    double yield;
    int stock;
};

inline void to_json(nlohmann::json& j, const Sample& s) {
    j = {{"id",s.id},{"name",s.name},
         {"avgProductionTime",s.avgProductionTime},
         {"yield",s.yield},{"stock",s.stock}};
}
inline void from_json(const nlohmann::json& j, Sample& s) {
    j.at("id").get_to(s.id);
    j.at("name").get_to(s.name);
    j.at("avgProductionTime").get_to(s.avgProductionTime);
    j.at("yield").get_to(s.yield);
    j.at("stock").get_to(s.stock);
}
