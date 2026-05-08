#pragma once
#include <optional>
#include <string>
#include "../model/Order.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"

class OrderController {
public:
    OrderController(SampleRepository& sampleRepo, OrderRepository& orderRepo);

    std::optional<Order> placeOrder(const std::string& sampleId,
                                    const std::string& customerName,
                                    int quantity);

private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;

    std::string generateOrderId() const;
    std::string currentTimestamp() const;
};
