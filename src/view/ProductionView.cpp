#include "ProductionView.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <thread>
#include <cstdio>
#include <conio.h>
#define NOMINMAX
#include <Windows.h>

static const int W         = 70;
static const int CLOCK_ROW = 1;
static const int CLOCK_COL = 44;

static void dline() { std::cout << "  " << std::string(W, '=') << "\n"; }
static void sline() { std::cout << "  " << std::string(W, '-') << "\n"; }

static int dispW(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        uint32_t cp = 0; int bytes = 1;
        if      (c < 0x80) { cp = c;        bytes = 1; }
        else if (c < 0xE0) { cp = c & 0x1F; bytes = 2; }
        else if (c < 0xF0) { cp = c & 0x0F; bytes = 3; }
        else               { cp = c & 0x07; bytes = 4; }
        for (int j = 1; j < bytes && i+j < s.size(); ++j)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i+j]) & 0x3F);
        i += bytes;
        bool wide = (cp >= 0xAC00 && cp <= 0xD7A3) ||
                    (cp >= 0x3040 && cp <= 0xA4CF) ||
                    (cp >= 0x1100 && cp <= 0x115F);
        w += wide ? 2 : 1;
    }
    return w;
}
static std::string padR(const std::string& s, int width) {
    int pad = width - dispW(s);
    return s + (pad > 0 ? std::string(pad, ' ') : "");
}

static std::string currentTimeStr() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

static void moveCursor(int x, int y) {
    COORD pos{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// ISO 8601 문자열 → time_t 파싱
static time_t parseTs(const std::string& ts) {
    std::tm tm = {};
    if (sscanf_s(ts.c_str(), "%d-%d-%dT%d:%d:%d",
                 &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                 &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
        return -1;
    tm.tm_year -= 1900; tm.tm_mon -= 1; tm.tm_isdst = -1;
    return mktime(&tm);
}

// 초 → "X분 XX초" 형식
static std::string toMinSec(double totalMinutes) {
    int totalSec = static_cast<int>(std::max(0.0, totalMinutes) * 60);
    int m = totalSec / 60;
    int s = totalSec % 60;
    std::ostringstream oss;
    oss << m << "분 " << std::setfill('0') << std::setw(2) << s << "초";
    return oss.str();
}

static void drawScreen(const std::string& path,
                        const std::optional<ProductionJob>& current,
                        const std::vector<ProductionJob>& queue) {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";  // row 1 → 시각 덮어쓰기
    sline();
    std::cout << "  [ 생산라인 조회 ]  FIFO 방식\n\n";

    std::cout << "  현재 생산 중\n";
    sline();
    if (!current.has_value()) {
        std::cout << "  생산 중인 작업 없음\n";
    } else {
        const auto& j = *current;

        // enqueuedAt으로 경과 초 계산
        double elapsedMin = 0.0;
        if (!j.enqueuedAt.empty()) {
            time_t start = parseTs(j.enqueuedAt);
            if (start != -1)
                elapsedMin = difftime(time(nullptr), start) / 60.0;
        }
        double remaining = std::max(0.0, j.totalTime - elapsedMin);
        int pct = j.totalTime > 0
            ? std::min(100, static_cast<int>(elapsedMin / j.totalTime * 100))
            : 100;

        const int L = 14;
        std::cout << "  " << padR("주문번호",   L) << ": " << j.orderId << "\n";
        std::cout << "  " << padR("시료",       L) << ": " << j.sampleId << "\n";
        std::cout << "  " << padR("부족분",     L) << ": " << j.shortage << " ea\n";
        std::cout << "  " << padR("실생산량",   L) << ": " << j.actualProduction << " ea\n";
        std::cout << "  " << padR("총생산시간", L) << ": " << toMinSec(j.totalTime) << "\n";
        std::cout << "  " << padR("경과시간",   L) << ": " << toMinSec(elapsedMin) << "\n";
        std::cout << "  " << padR("잔여시간",   L) << ": " << toMinSec(remaining)
                  << "  (" << pct << "%)\n";
    }

    int waitCount = current.has_value()
        ? static_cast<int>(queue.size()) - 1
        : static_cast<int>(queue.size());
    std::cout << "\n  대기 중인 작업  " << waitCount << "건\n";
    sline();
    if (waitCount == 0) {
        std::cout << "  대기 없음\n";
    } else {
        std::cout << "  " << std::left
                  << std::setw(5)  << "순서"
                  << padR("주문번호", 22)
                  << padR("시료ID",  10)
                  << std::setw(10) << "실생산량"
                  << "총시간\n";
        int seq = 1; bool first = current.has_value();
        for (const auto& j : queue) {
            if (first) { first = false; continue; }
            std::cout << "  " << std::left
                      << std::setw(5)  << seq++
                      << padR(j.orderId, 22)
                      << padR(j.sampleId, 10)
                      << std::setw(10) << (std::to_string(j.actualProduction) + " ea")
                      << toMinSec(j.totalTime) << "\n";
        }
    }

    std::cout << "\n";
    sline();
    std::cout << "  (계속하려면 아무 키나 누르세요)\n선택 > " << std::flush;

    // 시각 오버레이
    moveCursor(CLOCK_COL, CLOCK_ROW);
    std::cout << currentTimeStr() << std::flush;
    moveCursor(7, 19);  // "선택 > " 다음
}

void ProductionView::showStatus(const std::string& path,
                                 const std::optional<ProductionJob>& current,
                                 const std::vector<ProductionJob>& queue,
                                 double /*elapsedMinutes*/) const {
    drawScreen(path, current, queue);

    while (true) {
        for (int i = 0; i < 10; ++i) {
            if (_kbhit()) { _getch(); return; }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        // 1초마다 전체 화면 갱신 (경과/잔여시간 업데이트)
        drawScreen(path, current, queue);
    }
}
