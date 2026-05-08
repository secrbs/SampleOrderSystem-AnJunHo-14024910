#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"
#include "../model/Sample.h"

class ApprovalView {
public:
    // RESERVED 목록 표시 — 유효한 orderId 반환, 빈 문자열 = 뒤로
    std::string showReservedOrders(const std::string& path,
                                   const std::vector<Order>& orders) const;

    // 재고 확인 + 승인/거절 선택 — 'Y'/'y'=승인, 'N'/'n'=거절, 기타=취소
    char getApproveOrReject(const std::string& path,
                             const Order& order,
                             const Sample& sample) const;

    // 처리 결과 화면 (remainingStock: CONFIRMED 시 남은 재고, -1이면 미표시)
    void showResult(const std::string& path, const Order& order,
                    int remainingStock = -1) const;

    void showMessage(const std::string& msg) const;
};
