#pragma once
#include <map>
#include <vector>
#include <string>
#include "../model/Order.h"
#include "../controller/MonitoringController.h"

class MonitorView {
public:
    void showMonitoring(const std::string& path,
                        const std::map<OrderStatus, int>& summary,
                        const std::vector<StockInfo>& stocks) const;
};
