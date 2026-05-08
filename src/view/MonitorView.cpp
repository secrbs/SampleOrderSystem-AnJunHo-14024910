#include "MonitorView.h"
#include <iostream>
#include <iomanip>

void MonitorView::showOrderSummary(const std::map<OrderStatus, int>& summary) const {
    auto get = [&](OrderStatus s) {
        return summary.count(s) ? summary.at(s) : 0;
    };
    std::cout << "\n[ 주문 현황 ]\n";
    std::cout << "  RESERVED  : " << get(OrderStatus::RESERVED)  << "건\n";
    std::cout << "  CONFIRMED : " << get(OrderStatus::CONFIRMED)  << "건\n";
    std::cout << "  PRODUCING : " << get(OrderStatus::PRODUCING)  << "건\n";
    std::cout << "  RELEASE   : " << get(OrderStatus::RELEASE)    << "건\n";
}

void MonitorView::showStockInfo(const std::vector<StockInfo>& stocks) const {
    std::cout << "\n[ 재고 현황 ]\n";
    std::cout << "  " << std::left
              << std::setw(8)  << "ID"
              << std::setw(20) << "시료명"
              << std::setw(8)  << "재고"
              << std::setw(10) << "CONFIRMED"
              << "상태\n";
    std::cout << "  " << std::string(52, '-') << "\n";
    for (const auto& s : stocks) {
        std::string statusStr;
        switch (s.status) {
            case StockStatus::SURPLUS:  statusStr = "[여유]"; break;
            case StockStatus::SHORT:    statusStr = "[부족]"; break;
            case StockStatus::DEPLETED: statusStr = "[고갈]"; break;
        }
        std::cout << "  " << std::left
                  << std::setw(8)  << s.sampleId
                  << std::setw(20) << s.name
                  << std::setw(8)  << s.stock
                  << std::setw(10) << s.confirmedQuantity
                  << statusStr << "\n";
    }
}
