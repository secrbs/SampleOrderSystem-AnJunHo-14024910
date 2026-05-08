#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>

enum class OrderStatus { RESERVED, REJECTED, PRODUCING, CONFIRMED, RELEASE };

inline void to_json(nlohmann::json& j, const OrderStatus& s) {
    static const std::map<OrderStatus,std::string> m = {
        {OrderStatus::RESERVED,"RESERVED"},{OrderStatus::REJECTED,"REJECTED"},
        {OrderStatus::PRODUCING,"PRODUCING"},{OrderStatus::CONFIRMED,"CONFIRMED"},
        {OrderStatus::RELEASE,"RELEASE"}};
    j = m.at(s);
}
inline void from_json(const nlohmann::json& j, OrderStatus& s) {
    static const std::map<std::string,OrderStatus> m = {
        {"RESERVED",OrderStatus::RESERVED},{"REJECTED",OrderStatus::REJECTED},
        {"PRODUCING",OrderStatus::PRODUCING},{"CONFIRMED",OrderStatus::CONFIRMED},
        {"RELEASE",OrderStatus::RELEASE}};
    s = m.at(j.get<std::string>());
}

struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity;
    OrderStatus status;
    std::string createdAt;
    std::string updatedAt;
};

inline void to_json(nlohmann::json& j, const Order& o) {
    j = {{"orderId",o.orderId},{"sampleId",o.sampleId},
         {"customerName",o.customerName},{"quantity",o.quantity},
         {"status",o.status},{"createdAt",o.createdAt},{"updatedAt",o.updatedAt}};
}
inline void from_json(const nlohmann::json& j, Order& o) {
    j.at("orderId").get_to(o.orderId);   j.at("sampleId").get_to(o.sampleId);
    j.at("customerName").get_to(o.customerName); j.at("quantity").get_to(o.quantity);
    j.at("status").get_to(o.status);
    j.at("createdAt").get_to(o.createdAt); j.at("updatedAt").get_to(o.updatedAt);
}

struct ProductionJob {
    std::string orderId;
    std::string sampleId;
    int shortage;
    int actualProduction;
    double totalTime;
    std::string enqueuedAt;
};

inline void to_json(nlohmann::json& j, const ProductionJob& p) {
    j = {{"orderId",p.orderId},{"sampleId",p.sampleId},
         {"shortage",p.shortage},{"actualProduction",p.actualProduction},
         {"totalTime",p.totalTime},{"enqueuedAt",p.enqueuedAt}};
}
inline void from_json(const nlohmann::json& j, ProductionJob& p) {
    j.at("orderId").get_to(p.orderId);   j.at("sampleId").get_to(p.sampleId);
    j.at("shortage").get_to(p.shortage); j.at("actualProduction").get_to(p.actualProduction);
    j.at("totalTime").get_to(p.totalTime); j.at("enqueuedAt").get_to(p.enqueuedAt);
}
