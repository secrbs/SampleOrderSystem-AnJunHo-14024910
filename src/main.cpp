#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <map>
#include <string>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "controller/MainController.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/ApprovalController.h"
#include "controller/MonitoringController.h"
#include "controller/ProductionController.h"
#include "controller/ShipmentController.h"
#include "view/MainView.h"
#include "view/SampleView.h"
#include "view/OrderView.h"
#include "view/ApprovalView.h"
#include "view/MonitorView.h"
#include "view/ProductionView.h"
#include "view/ShipmentView.h"

// 메뉴 번호 → 이름 매핑 (경로 표시용)
static const std::map<int, std::string> MAIN_MENU_NAMES = {
    {1, "시료 관리"}, {2, "시료 주문"}, {3, "주문 승인/거절"},
    {4, "모니터링"}, {5, "생산라인 조회"}, {6, "출고 처리"}
};
static const std::map<int, std::string> SAMPLE_MENU_NAMES = {
    {1, "시료 등록"}, {2, "시료 목록"}, {3, "시료 검색"}
};
static const std::map<int, std::string> ORDER_MENU_NAMES = {
    {1, "주문 접수"}
};

static void runApproval(SampleRepository& sr, OrderRepository& or_,
                         const std::string& path) {
    ApprovalController ctrl(sr, or_);
    ApprovalView       view;

    while (true) {
        auto reserved = ctrl.getReservedOrders();
        std::string orderId = view.showReservedOrders(path, reserved);
        if (orderId.empty()) break;  // 취소 → 메인으로

        // 해당 주문 존재 확인
        auto orders = or_.findAll();
        auto it = std::find_if(orders.begin(), orders.end(),
            [&orderId](const Order& o){ return o.orderId == orderId; });
        if (it == orders.end() || it->status != OrderStatus::RESERVED) {
            view.showMessage("유효한 RESERVED 주문번호가 아닙니다.");
            std::cout << "\n"; _getch(); continue;
        }

        auto sample = sr.findById(it->sampleId);
        if (!sample.has_value()) {
            view.showMessage("시료 정보를 찾을 수 없습니다.");
            std::cout << "\n"; _getch(); continue;
        }
        char action = view.getApproveOrReject(path, *it, *sample);
        if (action == 'Y' || action == 'y') {
            ctrl.approve(orderId);
        } else if (action == 'N' || action == 'n') {
            ctrl.reject(orderId);
        } else {
            continue;  // 취소
        }

        // 결과 표시 (갱신된 주문 + 남은 재고)
        auto updated = or_.findAll();
        auto uit = std::find_if(updated.begin(), updated.end(),
            [&orderId](const Order& o){ return o.orderId == orderId; });
        if (uit != updated.end()) {
            int remaining = -1;
            if (uit->status == OrderStatus::CONFIRMED) {
                auto updSample = sr.findById(uit->sampleId);
                if (updSample.has_value()) remaining = updSample->stock;
            }
            view.showResult(path, *uit, remaining);
        }
    }
}

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

static void runOrderPlacement(SampleRepository& sr, OrderRepository& or_,
                               const std::string& path) {
    OrderController ctrl(sr, or_);
    OrderView       view;

    // 서브메뉴 없이 바로 주문 입력 진행
    auto input = view.inputOrder(path);
    if (!input.has_value()) return;  // 취소

    auto sample = sr.findById(input->sampleId);
    if (!sample.has_value()) {
        view.showMessage("존재하지 않는 시료 ID입니다.");
        std::cout << "\n"; _getch(); return;
    }
    if (input->quantity <= 0) {
        view.showMessage("주문 수량은 1 이상이어야 합니다.");
        std::cout << "\n"; _getch(); return;
    }

    if (!view.confirmOrder(path, *sample, *input)) return;

    auto order = ctrl.placeOrder(input->sampleId, input->customerName, input->quantity);
    if (order.has_value())
        view.showResult(path, *order, *sample);
    else {
        view.showMessage("주문 접수에 실패했습니다.");
        std::cout << "\n"; _getch();
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
        if (choice == -2) {          // 빈칸 → 종료 확인
            if (mainView.confirmExit()) break;
            else continue;
        }
        if (!mainCtrl.handleMenu(choice)) break;

        // 생산 자동 완료 체크
        { ProductionController pc(sampleRepo, orderRepo); while (pc.checkAndAutoComplete()) {} }

        std::string path = buildPath("선택", choice, MAIN_MENU_NAMES);

        switch (choice) {
            case 1: runSampleManagement(sampleRepo, path); break;
            case 2: runOrderPlacement(sampleRepo, orderRepo, path); break;
            case 3: runApproval(sampleRepo, orderRepo, path); break;
            case 4: {
                MonitoringController mc(sampleRepo, orderRepo);
                MonitorView mv;
                mv.showMonitoring(path, mc.getOrderSummary(), mc.getStockInfo());
                break;
            }
            case 5: {
                ProductionController pc(sampleRepo, orderRepo);
                ProductionView pv;
                pv.showStatus(path, pc.getCurrentJob(), pc.getQueue(), pc.getElapsedMinutes());
                break;
            }
            case 6: {
                ShipmentController sc(sampleRepo, orderRepo);
                ShipmentView sv;
                while (true) {
                    auto confirmed = sc.getConfirmedOrders();
                    std::string orderId = sv.showConfirmedOrders(path, confirmed);
                    if (orderId.empty()) break;
                    if (sc.release(orderId)) {
                        auto orders = orderRepo.findAll();
                        auto it = std::find_if(orders.begin(), orders.end(),
                            [&orderId](const Order& o){ return o.orderId == orderId; });
                        int remaining = -1;
                        auto sample = sampleRepo.findById(it->sampleId);
                        if (sample.has_value()) remaining = sample->stock;
                        if (it != orders.end()) sv.showResult(path, *it, remaining);
                    } else {
                        sv.showMessage("출고 실패.");
                        _getch();
                    }
                }
                break;
            }
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
