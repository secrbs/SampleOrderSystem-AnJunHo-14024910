#pragma once
#include <vector>
#include <string>
#include "../model/Order.h"

class ApprovalView {
public:
    void showReservedOrders(const std::vector<Order>& orders) const;
    std::string getOrderId() const;
    char getApproveOrReject() const;
    void showResult(const Order& order) const;
    void showMessage(const std::string& msg) const;
};
