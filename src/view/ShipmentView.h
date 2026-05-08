#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"

class ShipmentView {
public:
    // CONFIRMED 목록 표시 후 선택 — 빈 문자열 = 뒤로
    std::string showConfirmedOrders(const std::string& path,
                                    const std::vector<Order>& orders) const;

    // 출고 결과 화면
    void showResult(const std::string& path, const Order& order,
                    int remainingStock) const;

    void showMessage(const std::string& msg) const;
};
