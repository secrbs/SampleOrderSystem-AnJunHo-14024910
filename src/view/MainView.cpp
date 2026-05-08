#include "MainView.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

void MainView::showMenu(const SystemStatus& status) const {
    // 현재 시각
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);

    std::cout << "\n============================================\n";
    std::cout << "     반도체 시료 생산주문관리 시스템\n";
    std::cout << "============================================\n";
    std::cout << "  시각    : " << buf << "\n";
    std::cout << "  등록 시료: " << status.sampleCount << "종"
              << "  총 재고: "  << status.totalStock   << " ea\n";
    std::cout << "  전체 주문: " << status.orderCount << "건"
              << "  생산 대기: " << status.productionQueueCount << "건\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "  [1] 시료 관리        [2] 시료 주문\n";
    std::cout << "  [3] 주문 승인/거절   [4] 모니터링\n";
    std::cout << "  [5] 생산라인 조회    [6] 출고 처리\n";
    std::cout << "  [0] 종료\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "선택 > ";
}

int MainView::getChoice() const {
    int choice = -1;
    std::cin >> choice;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(1024, '\n');
        return -1;
    }
    return choice;
}

void MainView::showStub(int menuNum) const {
    std::cout << "\n  [" << menuNum << "] 기능은 준비 중입니다.\n";
}

void MainView::showInvalidInput() const {
    std::cout << "\n  올바른 메뉴 번호를 입력하세요 (0~6).\n";
}
