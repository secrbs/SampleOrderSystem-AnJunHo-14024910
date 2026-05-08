#include "MainController.h"

MainController::MainController(SampleRepository& sampleRepo, OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

// choice == 0 이면 false(종료), 그 외 true(계속)
bool MainController::handleMenu(int choice) {
    return choice != 0;
}

SystemStatus MainController::getSystemStatus() const {
    return {
        sampleRepo_.count(),
        sampleRepo_.totalStock(),
        orderRepo_.count(),
        orderRepo_.productionQueueCount()
    };
}
