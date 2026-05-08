#include "ProductionController.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
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

double ProductionController::getElapsedMinutes() const {
    auto job = getCurrentJob();
    if (!job.has_value() || job->enqueuedAt.empty()) return 0.0;
    time_t start = parseTimestamp(job->enqueuedAt);
    if (start == -1) return 0.0;
    return difftime(time(nullptr), start) / 60.0;
}

bool ProductionController::checkAndAutoComplete() {
    auto job = getCurrentJob();
    if (!job.has_value() || job->enqueuedAt.empty()) return false;
    double elapsed = getElapsedMinutes();
    if (elapsed >= job->totalTime)
        return completeCurrentJob();
    return false;
}

bool ProductionController::completeCurrentJob() {
    auto queue = orderRepo_.getQueue();
    if (queue.empty()) return false;

    const auto& job = queue.front();

    auto sample = sampleRepo_.findById(job.sampleId);
    if (!sample.has_value()) return false;
    Sample updated = *sample;
    updated.stock += job.actualProduction;
    sampleRepo_.update(updated);

    auto orders = orderRepo_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&job](const Order& o) { return o.orderId == job.orderId; });
    if (it != orders.end()) {
        it->status    = OrderStatus::CONFIRMED;
        it->updatedAt = currentTimestamp();
        orderRepo_.update(*it);
    }

    orderRepo_.dequeueJob();
    return true;
}

time_t ProductionController::parseTimestamp(const std::string& ts) const {
    std::tm tm = {};
    if (sscanf_s(ts.c_str(), "%d-%d-%dT%d:%d:%d",
                 &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                 &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
        return -1;
    tm.tm_year -= 1900;
    tm.tm_mon  -= 1;
    tm.tm_isdst = -1;
    return mktime(&tm);
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
