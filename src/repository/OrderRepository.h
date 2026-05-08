#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"

class OrderRepository {
public:
    OrderRepository(const std::string& orderPath, const std::string& queuePath);
    std::vector<Order> findAll() const;
    int count() const;
    void save(const Order& o);
    bool update(const Order& o);
    void enqueueJob(const ProductionJob& job);
    void dequeueJob();
    std::vector<ProductionJob> getQueue() const;
    int productionQueueCount() const;
private:
    std::string orderPath_, queuePath_;
    std::vector<Order> orders_;
    std::vector<ProductionJob> queue_;
    void load();
    void persist() const;
    void persistQueue() const;
};
