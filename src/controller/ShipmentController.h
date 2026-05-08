#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"
#include "../repository/OrderRepository.h"

class ShipmentController {
public:
    explicit ShipmentController(OrderRepository& orderRepo);

    std::vector<Order> getConfirmedOrders() const;
    bool release(const std::string& orderId);

private:
    OrderRepository& orderRepo_;
    std::string currentTimestamp() const;
};
