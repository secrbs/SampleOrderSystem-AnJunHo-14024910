#include "ApprovalController.h"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <ctime>

ApprovalController::ApprovalController(SampleRepository& sampleRepo,
                                        OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

std::vector<Order> ApprovalController::getReservedOrders() const {
    std::vector<Order> result;
    for (const auto& o : orderRepo_.findAll())
        if (o.status == OrderStatus::RESERVED)
            result.push_back(o);
    return result;
}

bool ApprovalController::approve(const std::string& orderId) {
    auto orders = orderRepo_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&orderId](const Order& o){ return o.orderId == orderId; });
    if (it == orders.end()) return false;
    if (it->status != OrderStatus::RESERVED) return false;

    auto sample = sampleRepo_.findById(it->sampleId);
    if (!sample.has_value()) return false;

    it->updatedAt = currentTimestamp();

    if (sample->stock >= it->quantity) {
        // 재고 충분 → CONFIRMED (재고 차감은 출고 시 수행)
        it->status = OrderStatus::CONFIRMED;
    } else {
        // 재고 부족 → PRODUCING, 생산라인 큐 등록
        it->status = OrderStatus::PRODUCING;
        ProductionJob job;
        job.orderId          = it->orderId;
        job.sampleId         = it->sampleId;
        job.shortage         = it->quantity - sample->stock;
        job.actualProduction = static_cast<int>(
            std::ceil(job.shortage / (sample->yield * 0.9)));
        job.totalTime        = sample->avgProductionTime * job.actualProduction;
        job.enqueuedAt       = currentTimestamp();
        orderRepo_.enqueueJob(job);
    }

    orderRepo_.update(*it);
    return true;
}

bool ApprovalController::reject(const std::string& orderId) {
    auto orders = orderRepo_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&orderId](const Order& o){ return o.orderId == orderId; });
    if (it == orders.end()) return false;
    if (it->status != OrderStatus::RESERVED) return false;

    it->status    = OrderStatus::REJECTED;
    it->updatedAt = currentTimestamp();
    orderRepo_.update(*it);
    return true;
}

std::string ApprovalController::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return buf;
}
