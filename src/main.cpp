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
        auto status = controller.getSystemStatus();
        view.showMenu(status);
        int choice = view.getChoice();
        if (!controller.handleMenu(choice)) break;
    }
    return 0;
}
