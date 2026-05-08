#include "ApprovalView.h"
#include <iostream>
#include <iomanip>

void ApprovalView::showReservedOrders(const std::vector<Order>& orders) const {
    if (orders.empty()) {
        std::cout << "  승인 대기 중인 주문이 없습니다.\n";
        return;
    }
    std::cout << "\n[ 승인 대기 주문 (RESERVED) ]\n";
    std::cout << "  " << std::left
              << std::setw(22) << "주문번호"
              << std::setw(14) << "고객"
              << std::setw(10) << "시료ID"
              << "수량\n";
    std::cout << "  " << std::string(56, '-') << "\n";
    for (const auto& o : orders)
        std::cout << "  " << std::left
                  << std::setw(22) << o.orderId
                  << std::setw(14) << o.customerName
                  << std::setw(10) << o.sampleId
                  << o.quantity << " ea\n";
}

std::string ApprovalView::getOrderId() const {
    std::cout << "\n  주문번호 > ";
    std::string id;
    std::cin >> id;
    return id;
}

char ApprovalView::getApproveOrReject() const {
    std::cout << "  [Y] 승인  [N] 거절  [0] 취소 > ";
    char ch;
    std::cin >> ch;
    return ch;
}

void ApprovalView::showResult(const Order& order) const {
    std::cout << "\n  처리 완료.\n";
    std::cout << "  주문번호 : " << order.orderId << "\n";
    std::string statusStr;
    switch (order.status) {
        case OrderStatus::CONFIRMED: statusStr = "RESERVED → CONFIRMED (재고 충분)"; break;
        case OrderStatus::PRODUCING: statusStr = "RESERVED → PRODUCING (생산 중)";  break;
        case OrderStatus::REJECTED:  statusStr = "RESERVED → REJECTED";              break;
        default: statusStr = "알 수 없음";
    }
    std::cout << "  상태     : " << statusStr << "\n";
}

void ApprovalView::showMessage(const std::string& msg) const {
    std::cout << "  " << msg << "\n";
}
