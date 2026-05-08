#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"

class OrderRepository {
public:
    OrderRepository(const std::string& orderFilePath,
                    const std::string& queueFilePath);

    std::vector<Order> findAll() const;
    int count() const;
    int productionQueueCount() const;

    void save(const Order& order);
    bool update(const Order& order);

    void enqueueJob(const ProductionJob& job);
    void dequeueJob();
    std::vector<ProductionJob> getQueue() const;

private:
    std::string orderFilePath_;
    std::string queueFilePath_;
    std::vector<Order> orders_;
    std::vector<ProductionJob> queue_;

    void load();
    void persist() const;
    void persistQueue() const;
};
