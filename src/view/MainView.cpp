#include "MainView.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>
#include <conio.h>
#include <Windows.h>

static const int WIDTH = 62;

void MainView::moveCursor(int x, int y) const {
    COORD pos{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

std::string MainView::currentTime() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

void MainView::updateClock() const {
    moveCursor(CLOCK_COL, CLOCK_ROW);
    std::cout << currentTime() << std::flush;
}

void MainView::drawScreen(const SystemStatus& status) const {
    // 화면 지우기
    system("cls");

    auto line  = [](char c) { std::cout << "  " << std::string(WIDTH, c) << "\n"; };
    auto dline = [&]()      { line('='); };
    auto sline = [&]()      { line('-'); };

    // 헤더
    dline();
    std::cout << "  반도체 시료 생산주문관리 시스템"
              << std::setw(WIDTH - 30) << "" << "\n";
    dline();

    // 시스템 현황
    std::cout << "  등록 시료  " << std::setw(4) << status.sampleCount << " 종"
              << "  │  총 재고   " << std::setw(6) << status.totalStock << " ea\n";
    std::cout << "  전체 주문  " << std::setw(4) << status.orderCount  << " 건"
              << "  │  생산 대기 " << std::setw(6) << status.productionQueueCount << " 건\n";
    sline();

    // 메뉴
    std::cout << "  [1] 시료 관리          [2] 시료 주문\n";
    std::cout << "  [3] 주문 승인/거절     [4] 모니터링\n";
    std::cout << "  [5] 생산라인 조회      [6] 출고 처리\n";
    std::cout << "  [0] 종료\n";
    sline();

    // 입력 프롬프트
    std::cout << "선택 > " << std::flush;

    // 시각을 헤더 우측에 덮어쓰기
    updateClock();

    // 커서를 입력 프롬프트 위치로 복귀
    moveCursor(INPUT_COL, INPUT_ROW);
}

int MainView::getChoice() const {
    // _kbhit() 루프: 1초마다 시각 갱신, 입력 오면 즉시 처리
    while (true) {
        for (int i = 0; i < 10; ++i) {    // 100ms × 10 = 1초
            if (_kbhit()) {
                char ch = static_cast<char>(_getch());
                std::cout << ch << "\n";
                if (ch >= '0' && ch <= '9') return ch - '0';
                if (ch == '\r' || ch == '\n') return -2;  // 빈칸 = 종료 확인
                return -1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        updateClock();
        moveCursor(INPUT_COL, INPUT_ROW);
    }
}

void MainView::showInvalidInput() const {
    moveCursor(0, INPUT_ROW + 1);
    std::cout << "  올바른 메뉴 번호를 입력하세요 (0~8).\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
}

bool MainView::confirmExit() const {
    moveCursor(0, INPUT_ROW + 1);
    std::cout << "  종료하시겠습니까? [Y/N] > ";
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    return (ch == 'Y' || ch == 'y');
}
