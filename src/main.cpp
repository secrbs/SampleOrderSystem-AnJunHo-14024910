#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "controller/MainController.h"
#include "view/MainView.h"

int main() {
    SampleRepository sampleRepo("data/samples.json");
    OrderRepository  orderRepo("data/orders.json", "data/production_queue.json");
    MainController   controller(sampleRepo, orderRepo);
    MainView         view;

    while (true) {
        view.showMenu(controller.getSystemStatus());
        int choice = view.getChoice();
        if (!controller.handleMenu(choice)) break;
        if (choice >= 1 && choice <= 6) view.showStub(choice);
        else view.showInvalidInput();
    }
    return 0;
}
