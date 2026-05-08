#include "OrderController.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

OrderController::OrderController(SampleRepository& sampleRepo, OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

std::optional<Order> OrderController::placeOrder(const std::string& sampleId,
                                                  const std::string& customerName,
                                                  int quantity) {
    if (!sampleRepo_.findById(sampleId).has_value()) return std::nullopt;
    if (quantity <= 0) return std::nullopt;

    Order order;
    order.orderId      = generateOrderId();
    order.sampleId     = sampleId;
    order.customerName = customerName;
    order.quantity     = quantity;
    order.status       = OrderStatus::RESERVED;
    order.createdAt    = currentTimestamp();
    order.updatedAt    = order.createdAt;

    orderRepo_.save(order);
    return order;
}

std::string OrderController::generateOrderId() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);

    char dateBuf[16];
    std::strftime(dateBuf, sizeof(dateBuf), "%Y%m%d", &tm);

    // 당일 주문 수 기반 순번
    int seq = 1;
    for (const auto& o : orderRepo_.findAll()) {
        if (o.orderId.find(dateBuf) != std::string::npos)
            ++seq;
    }

    std::ostringstream oss;
    oss << "ORD-" << dateBuf << "-" << std::setfill('0') << std::setw(4) << seq;
    return oss.str();
}

std::string OrderController::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return buf;
}
