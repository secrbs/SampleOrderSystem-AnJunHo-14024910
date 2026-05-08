#include "MainController.h"

MainController::MainController(SampleRepository& sampleRepo, OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

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
