#pragma once
#include <vector>
#include <optional>
#include "../model/Order.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"

class ProductionController {
public:
    ProductionController(SampleRepository& sampleRepo, OrderRepository& orderRepo);

    int    calcActualProduction(int shortage, double yield) const;
    double calcTotalTime(double avgTime, int actualProduction) const;

    std::vector<ProductionJob>   getQueue() const;
    std::optional<ProductionJob> getCurrentJob() const;
    double                       getElapsedMinutes() const;
    bool                         checkAndAutoComplete();
    bool                         completeCurrentJob();

private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;

    time_t parseTimestamp(const std::string& ts) const;
    std::string currentTimestamp() const;
};
