#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <map>
#include <string>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "controller/MainController.h"
#include "controller/SampleController.h"
#include "view/MainView.h"
#include "view/SampleView.h"

// 메뉴 번호 → 이름 매핑 (경로 표시용)
static const std::map<int, std::string> MAIN_MENU_NAMES = {
    {1, "시료 관리"}, {2, "시료 주문"}, {3, "주문 승인/거절"},
    {4, "모니터링"}, {5, "생산라인 조회"}, {6, "출고 처리"}
};
static const std::map<int, std::string> SAMPLE_MENU_NAMES = {
    {1, "시료 등록"}, {2, "시료 목록"}, {3, "시료 검색"}
};

static std::string buildPath(const std::string& parent, int ch,
                              const std::map<int, std::string>& names) {
    auto it = names.find(ch);
    std::string label = (it != names.end()) ? it->second : "";
    return parent + " > " + std::to_string(ch)
           + (label.empty() ? "" : ". " + label);
}

static void runSampleManagement(SampleRepository& sr, const std::string& path) {
    SampleController ctrl(sr);
    SampleView       view;

    while (true) {
        view.showMenu(path);
        int ch = view.getChoice();
        if (ch == 0) break;

        std::string subPath = buildPath(path, ch, SAMPLE_MENU_NAMES);

        if (ch == 1) {
            view.showRegisterForm(subPath);
            auto s = view.inputSample();
            if (!s.has_value()) {
                // 취소 → 시료 관리 메뉴로
            } else if (ctrl.registerSample(*s)) {
                view.showMessage("등록 완료.");
                std::cout << "\n"; _getch();
            } else {
                view.showMessage("등록 실패 (중복 ID 또는 유효하지 않은 수율).");
                std::cout << "\n"; _getch();
            }
        } else if (ch == 2) {
            view.showSamples(subPath, "시료 목록", ctrl.getAll());
        } else if (ch == 3) {
            // 결과 없으면 검색 입력 화면으로 재루프
            while (true) {
                view.showSearchForm(subPath);
                std::string kw = view.inputSearchKeyword();
                if (kw.empty()) break;  // 취소 → 시료 관리 메뉴로
                auto result = ctrl.searchByName(kw);
                if (result.empty()) {
                    view.showMessage("검색 결과가 없습니다. 다시 입력하세요.");
                    std::cout << "\n"; _getch();
                } else {
                    view.showSamples(subPath, "검색 결과", result);
                    break;
                }
            }
        } else {
            view.showMessage("올바른 메뉴를 선택하세요 (0~3).");
            _getch();
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci{ 1, FALSE };
    SetConsoleCursorInfo(hConsole, &ci);

    SampleRepository sampleRepo("data/samples.json");
    OrderRepository  orderRepo("data/orders.json", "data/production_queue.json");
    MainController   mainCtrl(sampleRepo, orderRepo);
    MainView         mainView;

    while (true) {
        mainView.drawScreen(mainCtrl.getSystemStatus());
        int choice = mainView.getChoice();
        if (!mainCtrl.handleMenu(choice)) break;

        std::string path = buildPath("선택", choice, MAIN_MENU_NAMES);

        switch (choice) {
            case 1: runSampleManagement(sampleRepo, path); break;
            // Phase 3~8: 이후 연결
            case 2: case 3: case 4: case 5: case 6: case 7: case 8:
                // 미구현 메뉴 — 메인으로 돌아감 (별도 메시지 없음)
                break;
            default:
                // 0~8 이외 입력 — 에러 표시
                mainView.showInvalidInput();
                break;
        }
    }

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD bottom{ 0, static_cast<SHORT>(csbi.dwCursorPosition.Y + 2) };
    SetConsoleCursorPosition(hConsole, bottom);
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &ci);
    return 0;
}
