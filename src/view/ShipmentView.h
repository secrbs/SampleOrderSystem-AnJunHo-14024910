#pragma once
#include <vector>
#include <string>
#include "../model/Order.h"

class ShipmentView {
public:
    void showConfirmedOrders(const std::vector<Order>& orders) const;
    std::string getOrderId() const;
    void showResult(const Order& order) const;
    void showMessage(const std::string& msg) const;
};
