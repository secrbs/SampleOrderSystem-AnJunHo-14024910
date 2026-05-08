#include "ProductionController.h"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <ctime>

ProductionController::ProductionController(SampleRepository& sampleRepo,
                                           OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

int ProductionController::calcActualProduction(int shortage, double yield) const {
    return static_cast<int>(std::ceil(shortage / (yield * 0.9)));
}

double ProductionController::calcTotalTime(double avgTime, int actualProduction) const {
    return avgTime * actualProduction;
}

std::vector<ProductionJob> ProductionController::getQueue() const {
    return orderRepo_.getQueue();
}

std::optional<ProductionJob> ProductionController::getCurrentJob() const {
    auto queue = orderRepo_.getQueue();
    if (queue.empty()) return std::nullopt;
    return queue.front();
}

bool ProductionController::completeCurrentJob() {
    auto queue = orderRepo_.getQueue();
    if (queue.empty()) return false;

    const auto& job = queue.front();

    // 재고 증가
    auto sample = sampleRepo_.findById(job.sampleId);
    if (!sample.has_value()) return false;
    Sample updated = *sample;
    updated.stock += job.actualProduction;
    sampleRepo_.update(updated);

    // 주문 상태 CONFIRMED
    auto orders = orderRepo_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&job](const Order& o) { return o.orderId == job.orderId; });
    if (it != orders.end()) {
        it->status    = OrderStatus::CONFIRMED;
        it->updatedAt = currentTimestamp();
        orderRepo_.update(*it);
    }

    // 큐에서 제거 (front dequeue)
    orderRepo_.dequeueJob();
    return true;
}

std::string ProductionController::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return buf;
}
