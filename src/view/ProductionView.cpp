#include "ProductionView.h"
#include <iostream>
#include <iomanip>

void ProductionView::showStatus(const std::optional<ProductionJob>& current,
                                 const std::vector<ProductionJob>& queue) const {
    std::cout << "\n[ 생산라인 현황 ] FIFO 방식\n";
    std::cout << "--------------------------------------------\n";

    if (!current.has_value()) {
        std::cout << "  현재 생산 중인 작업 없음\n";
    } else {
        const auto& j = *current;
        std::cout << "  현재 처리 중\n";
        std::cout << "    주문번호  : " << j.orderId << "\n";
        std::cout << "    시료      : " << j.sampleId << "\n";
        std::cout << "    부족분    : " << j.shortage << " ea\n";
        std::cout << "    실생산량  : " << j.actualProduction << " ea\n";
        std::cout << "    총생산시간: " << j.totalTime << " min\n";
    }

    std::cout << "\n  대기 중인 작업 (" << (queue.empty() ? 0 : queue.size() - 1) << "건)\n";
    bool first = true;
    for (const auto& j : queue) {
        if (first) { first = false; continue; }  // current는 제외
        std::cout << "    " << std::left
                  << std::setw(22) << j.orderId
                  << std::setw(10) << j.sampleId
                  << "실생산량 " << j.actualProduction << " ea\n";
    }
}

void ProductionView::showMenu() const {
    std::cout << "\n  [C] 생산 완료 처리  [0] 뒤로 > ";
}

int ProductionView::getChoice() const {
    char ch;
    std::cin >> ch;
    if (ch == 'C' || ch == 'c') return 1;
    if (ch == '0') return 0;
    return -1;
}

void ProductionView::showCompleteResult() const {
    std::cout << "  생산 완료 처리되었습니다. (PRODUCING → CONFIRMED)\n";
}

void ProductionView::showMessage(const std::string& msg) const {
    std::cout << "  " << msg << "\n";
}
