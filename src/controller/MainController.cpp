#include "MainController.h"
#include "../view/MainView.h"

MainController::MainController(SampleRepository& sampleRepo, OrderRepository& orderRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo) {}

bool MainController::handleMenu(int choice) {
    MainView view;
    switch (choice) {
        case 0: return false;
        case 1: case 2: case 3:
        case 4: case 5: case 6:
            view.showStub(choice);
            return true;
        default:
            view.showInvalidInput();
            return true;
    }
}

SystemStatus MainController::getSystemStatus() const {
    return {
        sampleRepo_.count(),
        sampleRepo_.totalStock(),
        orderRepo_.count(),
        orderRepo_.productionQueueCount()
    };
}
