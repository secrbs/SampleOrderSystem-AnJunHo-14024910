#include <Windows.h>
#include <iostream>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "controller/MainController.h"
#include "view/MainView.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 커서 숨기기
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci{ 1, FALSE };
    SetConsoleCursorInfo(hConsole, &ci);

    SampleRepository sampleRepo("data/samples.json");
    OrderRepository  orderRepo("data/orders.json", "data/production_queue.json");
    MainController   ctrl(sampleRepo, orderRepo);
    MainView         view;

    while (true) {
        view.drawScreen(ctrl.getSystemStatus());
        int choice = view.getChoice();
        if (!ctrl.handleMenu(choice)) break;
        if (choice < 1 || choice > 8) view.showInvalidInput();
        // 각 기능은 이후 Phase에서 연결
    }

    // 커서 복원
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &ci);
    return 0;
}
