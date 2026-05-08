#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"
#include "../model/Sample.h"

class OrderView {
public:
    void showMenu() const;
    int  getChoice() const;
    void inputOrderInfo(std::string& sampleId, std::string& customerName, int& quantity) const;
    bool confirmOrder(const Sample& sample, const std::string& customerName, int quantity) const;
    void showOrderResult(const Order& order) const;
    void showMessage(const std::string& msg) const;
};
