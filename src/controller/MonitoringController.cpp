#include "MonitoringController.h"

MonitoringController::MonitoringController(SampleRepository& sampleRepo,
                                           OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

std::map<OrderStatus, int> MonitoringController::getOrderSummary() const {
    std::map<OrderStatus, int> summary;
    for (const auto& o : orderRepo_.findAll()) {
        if (o.status == OrderStatus::REJECTED) continue;
        summary[o.status]++;
    }
    return summary;
}

std::vector<StockInfo> MonitoringController::getStockInfo() const {
    // CONFIRMED 주문 수량 합산 (시료별)
    std::map<std::string, int> confirmedQty;
    for (const auto& o : orderRepo_.findAll())
        if (o.status == OrderStatus::CONFIRMED)
            confirmedQty[o.sampleId] += o.quantity;

    std::vector<StockInfo> result;
    for (const auto& s : sampleRepo_.findAll()) {
        int confirmed = confirmedQty.count(s.id) ? confirmedQty[s.id] : 0;
        StockStatus status;
        if (s.stock == 0)
            status = StockStatus::DEPLETED;
        else if (s.stock < confirmed)
            status = StockStatus::SHORT;
        else
            status = StockStatus::SURPLUS;
        result.push_back({s.id, s.name, s.stock, confirmed, status});
    }
    return result;
}
