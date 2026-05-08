#pragma once
#include "../model/SystemStatus.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"

class MainController {
public:
    MainController(SampleRepository& sampleRepo, OrderRepository& orderRepo);
    bool handleMenu(int choice);        // false = 종료
    SystemStatus getSystemStatus() const;
private:
    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
};
