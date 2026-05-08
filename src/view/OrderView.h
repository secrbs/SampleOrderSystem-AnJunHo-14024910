#pragma once
#include <string>
#include <optional>
#include "../model/Order.h"
#include "../model/Sample.h"

struct OrderInput {
    std::string sampleId;
    std::string customerName;
    int quantity = 0;
};

class OrderView {
public:
    void showMenu(const std::string& path) const;
    int  getChoice() const;

    // 주문 입력 폼 — nullopt = 취소
    std::optional<OrderInput> inputOrder(const std::string& path) const;

    // 확인 화면 — true = 접수, false = 취소
    bool confirmOrder(const std::string& path,
                      const Sample& sample,
                      const OrderInput& input) const;

    // 결과 화면
    void showResult(const std::string& path, const Order& order,
                    const Sample& sample) const;

    void showMessage(const std::string& msg) const;
};
