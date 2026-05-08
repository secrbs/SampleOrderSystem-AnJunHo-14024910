#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"

class ShipmentController {
public:
    ShipmentController(SampleRepository& sampleRepo, OrderRepository& orderRepo);

    std::vector<Order> getConfirmedOrders() const;
    bool release(const std::string& orderId);

private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
    std::string currentTimestamp() const;
};
