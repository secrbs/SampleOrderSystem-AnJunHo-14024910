#pragma once
#include <map>
#include <vector>
#include "../model/Order.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"

enum class StockStatus { SURPLUS, SHORT, DEPLETED };

struct StockInfo {
    std::string sampleId;
    std::string name;
    int stock;
    int confirmedQuantity;
    StockStatus status;
};

class MonitoringController {
public:
    MonitoringController(SampleRepository& sampleRepo, OrderRepository& orderRepo);

    std::map<OrderStatus, int>  getOrderSummary() const;
    std::vector<StockInfo>      getStockInfo() const;

private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
};
