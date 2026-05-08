#pragma once
#include <map>
#include <vector>
#include "../model/Order.h"
#include "../controller/MonitoringController.h"

class MonitorView {
public:
    void showOrderSummary(const std::map<OrderStatus, int>& summary) const;
    void showStockInfo(const std::vector<StockInfo>& stocks) const;
};
