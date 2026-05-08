#include "ShipmentView.h"
#include <iostream>
#include <iomanip>

void ShipmentView::showConfirmedOrders(const std::vector<Order>& orders) const {
    if (orders.empty()) {
        std::cout << "  출고 가능한 주문이 없습니다.\n";
        return;
    }
    std::cout << "\n[ 출고 가능 주문 (CONFIRMED) ]\n";
    std::cout << "  " << std::left
              << std::setw(20) << "주문번호"
              << std::setw(14) << "고객"
              << std::setw(10) << "시료ID"
              << "수량\n";
    std::cout << "  " << std::string(54, '-') << "\n";
    for (const auto& o : orders)
        std::cout << "  " << std::left
                  << std::setw(20) << o.orderId
                  << std::setw(14) << o.customerName
                  << std::setw(10) << o.sampleId
                  << o.quantity << " ea\n";
}

std::string ShipmentView::getOrderId() const {
    std::cout << "\n  출고할 주문번호 > ";
    std::string id;
    std::cin >> id;
    return id;
}

void ShipmentView::showResult(const Order& order) const {
    std::cout << "\n  출고 처리 완료.\n";
    std::cout << "  주문번호 : " << order.orderId << "\n";
    std::cout << "  수량     : " << order.quantity << " ea\n";
    std::cout << "  상태     : CONFIRMED → RELEASE\n";
}

void ShipmentView::showMessage(const std::string& msg) const {
    std::cout << "  " << msg << "\n";
}
