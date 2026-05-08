#include <Windows.h>
#include <iostream>
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

// [1] 시료 관리
static void runSampleManagement(SampleRepository& sr) {
    SampleController ctrl(sr);
    SampleView       view;
    while (true) {
        view.showMenu();
        int ch = view.getChoice();
        if (ch == 0) break;
        if (ch == 1) {
            Sample s = view.inputSample();
            if (ctrl.registerSample(s)) view.showMessage("등록 완료.");
            else                         view.showMessage("등록 실패 (중복 ID 또는 유효하지 않은 수율).");
        } else if (ch == 2) {
            view.showSamples(ctrl.getAll());
        } else if (ch == 3) {
            std::string kw = view.inputSearchKeyword();
            auto result = ctrl.searchByName(kw);
            if (result.empty()) view.showMessage("검색 결과 없음.");
            else                view.showSamples(result);
        } else {
            view.showMessage("올바른 메뉴를 선택하세요.");
        }
    }
}

// [2] 시료 주문
static void runOrderPlacement(SampleRepository& sr, OrderRepository& or_) {
    OrderController ctrl(sr, or_);
    SampleController sampleCtrl(sr);
    OrderView        view;
    std::string sampleId, customerName;
    int quantity = 0;
    view.inputOrderInfo(sampleId, customerName, quantity);
    auto sample = sampleCtrl.findById(sampleId);
    if (!sample.has_value()) { view.showMessage("존재하지 않는 시료 ID입니다."); return; }
    if (!view.confirmOrder(*sample, customerName, quantity)) { view.showMessage("취소되었습니다."); return; }
    auto order = ctrl.placeOrder(sampleId, customerName, quantity);
    if (order.has_value()) view.showOrderResult(*order);
    else                   view.showMessage("주문 접수 실패.");
}

// [3] 주문 승인/거절
static void runApproval(SampleRepository& sr, OrderRepository& or_) {
    ApprovalController ctrl(sr, or_);
    ApprovalView       view;
    auto reserved = ctrl.getReservedOrders();
    view.showReservedOrders(reserved);
    if (reserved.empty()) return;
    std::string orderId = view.getOrderId();
    char action = view.getApproveOrReject();
    bool ok = false;
    if (action == 'Y' || action == 'y') ok = ctrl.approve(orderId);
    else if (action == 'N' || action == 'n') ok = ctrl.reject(orderId);
    else { view.showMessage("취소되었습니다."); return; }
    if (!ok) { view.showMessage("처리 실패 (RESERVED 상태가 아니거나 존재하지 않는 주문)."); return; }
    auto orders = or_.findAll();
    for (const auto& o : orders)
        if (o.orderId == orderId) { view.showResult(o); break; }
}

// [4] 모니터링
static void runMonitoring(SampleRepository& sr, OrderRepository& or_) {
    MonitoringController ctrl(sr, or_);
    MonitorView          view;
    view.showOrderSummary(ctrl.getOrderSummary());
    view.showStockInfo(ctrl.getStockInfo());
}

// [5] 생산라인 조회
static void runProductionLine(SampleRepository& sr, OrderRepository& or_) {
    ProductionController ctrl(sr, or_);
    ProductionView       view;
    while (true) {
        view.showStatus(ctrl.getCurrentJob(), ctrl.getQueue());
        view.showMenu();
        int ch = view.getChoice();
        if (ch == 0) break;
        if (ch == 1) {
            if (!ctrl.getCurrentJob().has_value()) { view.showMessage("처리 중인 작업이 없습니다."); continue; }
            ctrl.completeCurrentJob();
            view.showCompleteResult();
        }
    }
}

// [6] 출고 처리
static void runShipment(OrderRepository& or_) {
    ShipmentController ctrl(or_);
    ShipmentView       view;
    view.showConfirmedOrders(ctrl.getConfirmedOrders());
    if (ctrl.getConfirmedOrders().empty()) return;
    std::string orderId = view.getOrderId();
    if (ctrl.release(orderId)) {
        auto orders = or_.findAll();
        for (const auto& o : orders)
            if (o.orderId == orderId) { view.showResult(o); break; }
    } else {
        view.showMessage("출고 실패 (CONFIRMED 상태가 아니거나 존재하지 않는 주문).");
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    SampleRepository sampleRepo("data/samples.json");
    OrderRepository  orderRepo("data/orders.json", "data/production_queue.json");
    MainController   mainCtrl(sampleRepo, orderRepo);
    MainView         mainView;

    while (true) {
        mainView.showMenu(mainCtrl.getSystemStatus());
        int choice = mainView.getChoice();
        switch (choice) {
            case 0: std::cout << "\n  종료합니다.\n"; return 0;
            case 1: runSampleManagement(sampleRepo);         break;
            case 2: runOrderPlacement(sampleRepo, orderRepo); break;
            case 3: runApproval(sampleRepo, orderRepo);       break;
            case 4: runMonitoring(sampleRepo, orderRepo);     break;
            case 5: runProductionLine(sampleRepo, orderRepo); break;
            case 6: runShipment(orderRepo);                   break;
            default: mainView.showInvalidInput();              break;
        }
    }
}
