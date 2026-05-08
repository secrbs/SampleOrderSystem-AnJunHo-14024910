#include "ProductionView.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void ProductionView::showStatus(const std::optional<ProductionJob>& current,
                                 const std::vector<ProductionJob>& queue,
                                 double elapsedMinutes) const {
    std::cout << "\n[ 생산라인 현황 ] FIFO 방식\n";
    std::cout << "--------------------------------------------\n";

    if (!current.has_value()) {
        std::cout << "  현재 생산 중인 작업 없음\n";
    } else {
        const auto& j = *current;
        double remaining = std::max(0.0, j.totalTime - elapsedMinutes);
        int progressPct  = (j.totalTime > 0)
            ? static_cast<int>(elapsedMinutes / j.totalTime * 100.0)
            : 100;
        progressPct = std::min(progressPct, 100);

        std::cout << "  [ 현재 생산 중 ]\n";
        std::cout << "    주문번호  : " << j.orderId << "\n";
        std::cout << "    시료      : " << j.sampleId << "\n";
        std::cout << "    부족분    : " << j.shortage << " ea\n";
        std::cout << "    실생산량  : " << j.actualProduction << " ea\n";
        std::cout << "    총생산시간: " << std::fixed << std::setprecision(1)
                  << j.totalTime << " min\n";
        std::cout << "    경과시간  : " << std::fixed << std::setprecision(1)
                  << elapsedMinutes << " min\n";
        std::cout << "    잔여시간  : " << std::fixed << std::setprecision(1)
                  << remaining << " min\n";
        std::cout << "    진행률    : " << progressPct << "%\n";
    }

    int waitCount = current.has_value() ? static_cast<int>(queue.size()) - 1
                                        : static_cast<int>(queue.size());
    std::cout << "\n  [ 대기 중인 작업 : " << waitCount << "건 ]\n";
    bool first = current.has_value();
    for (const auto& j : queue) {
        if (first) { first = false; continue; }
        std::cout << "    " << std::left
                  << std::setw(22) << j.orderId
                  << std::setw(10) << j.sampleId
                  << "실생산량 " << j.actualProduction << " ea  "
                  << "(" << std::fixed << std::setprecision(1)
                  << j.totalTime << " min)\n";
    }
    std::cout << "\n  (뒤로: 아무 키나 누르세요)\n";
}

void ProductionView::showMessage(const std::string& msg) const {
    std::cout << "  " << msg << "\n";
}
