#include "ShipmentController.h"
#include <algorithm>
#include <chrono>
#include <ctime>

ShipmentController::ShipmentController(OrderRepository& orderRepo)
    : orderRepo_(orderRepo) {}

std::vector<Order> ShipmentController::getConfirmedOrders() const {
    std::vector<Order> result;
    for (const auto& o : orderRepo_.findAll())
        if (o.status == OrderStatus::CONFIRMED)
            result.push_back(o);
    return result;
}

bool ShipmentController::release(const std::string& orderId) {
    auto orders = orderRepo_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&orderId](const Order& o) { return o.orderId == orderId; });
    if (it == orders.end()) return false;
    if (it->status != OrderStatus::CONFIRMED) return false;

    it->status    = OrderStatus::RELEASE;
    it->updatedAt = currentTimestamp();
    orderRepo_.update(*it);
    return true;
}

std::string ShipmentController::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return buf;
}
