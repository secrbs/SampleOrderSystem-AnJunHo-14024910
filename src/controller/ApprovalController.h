#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"

class ApprovalController {
public:
    ApprovalController(SampleRepository& sampleRepo, OrderRepository& orderRepo);

    std::vector<Order> getReservedOrders() const;
    bool approve(const std::string& orderId);
    bool reject(const std::string& orderId);

private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;

    std::string currentTimestamp() const;
};
