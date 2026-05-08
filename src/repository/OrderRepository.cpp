#include "OrderRepository.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>

OrderRepository::OrderRepository(const std::string& orderFilePath,
                                 const std::string& queueFilePath)
    : orderFilePath_(orderFilePath), queueFilePath_(queueFilePath) {
    load();
}

std::vector<Order> OrderRepository::findAll() const {
    return orders_;
}

int OrderRepository::count() const {
    return static_cast<int>(orders_.size());
}

int OrderRepository::productionQueueCount() const {
    return static_cast<int>(queue_.size());
}

void OrderRepository::save(const Order& order) {
    orders_.push_back(order);
    persist();
}

bool OrderRepository::update(const Order& order) {
    auto it = std::find_if(orders_.begin(), orders_.end(),
        [&order](const Order& o) { return o.orderId == order.orderId; });
    if (it == orders_.end()) return false;
    *it = order;
    persist();
    return true;
}

void OrderRepository::load() {
    auto loadFile = [](const std::string& path, auto& vec) {
        std::ifstream f(path);
        if (!f.is_open()) { vec.clear(); return; }
        try {
            nlohmann::json j;
            f >> j;
            vec = j.get<std::decay_t<decltype(vec)>>();
        } catch (...) {
            vec.clear();
        }
    };
    loadFile(orderFilePath_, orders_);
    loadFile(queueFilePath_, queue_);
}

void OrderRepository::enqueueJob(const ProductionJob& job) {
    queue_.push_back(job);
    persistQueue();
}

std::vector<ProductionJob> OrderRepository::getQueue() const {
    return queue_;
}

void OrderRepository::persist() const {
    std::ofstream f(orderFilePath_);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + orderFilePath_);
    f << nlohmann::json(orders_).dump(2);
}

void OrderRepository::persistQueue() const {
    std::ofstream f(queueFilePath_);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + queueFilePath_);
    f << nlohmann::json(queue_).dump(2);
}
