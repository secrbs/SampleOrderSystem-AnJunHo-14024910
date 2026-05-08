#pragma once
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/SystemStatus.h"

class MainController {
public:
    MainController(SampleRepository& sampleRepo, OrderRepository& orderRepo);

    bool handleMenu(int choice);  // false = 종료
    SystemStatus getSystemStatus() const;

private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
};
