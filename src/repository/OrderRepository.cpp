#include "OrderRepository.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>

OrderRepository::OrderRepository(const std::string& orderPath, const std::string& queuePath)
    : orderPath_(orderPath), queuePath_(queuePath) { load(); }

std::vector<Order> OrderRepository::findAll() const { return orders_; }
int OrderRepository::count() const { return static_cast<int>(orders_.size()); }
int OrderRepository::productionQueueCount() const { return static_cast<int>(queue_.size()); }
std::vector<ProductionJob> OrderRepository::getQueue() const { return queue_; }

void OrderRepository::save(const Order& o) { orders_.push_back(o); persist(); }

bool OrderRepository::update(const Order& o) {
    auto it = std::find_if(orders_.begin(), orders_.end(),
        [&o](const Order& x){ return x.orderId == o.orderId; });
    if (it == orders_.end()) return false;
    *it = o; persist(); return true;
}

void OrderRepository::enqueueJob(const ProductionJob& job) {
    queue_.push_back(job); persistQueue();
}

void OrderRepository::dequeueJob() {
    if (!queue_.empty()) { queue_.erase(queue_.begin()); persistQueue(); }
}

void OrderRepository::load() {
    auto loadFile = [](const std::string& path, auto& vec) {
        std::ifstream f(path);
        if (!f.is_open()) { vec.clear(); return; }
        try { nlohmann::json j; f >> j; vec = j.get<std::decay_t<decltype(vec)>>(); }
        catch (...) { vec.clear(); }
    };
    loadFile(orderPath_, orders_);
    loadFile(queuePath_, queue_);
}

void OrderRepository::persist() const {
    std::ofstream f(orderPath_);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + orderPath_);
    f << nlohmann::json(orders_).dump(2);
}

void OrderRepository::persistQueue() const {
    std::ofstream f(queuePath_);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + queuePath_);
    f << nlohmann::json(queue_).dump(2);
}
