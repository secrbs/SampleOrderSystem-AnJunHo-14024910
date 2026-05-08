#include "MonitorView.h"
#include <iostream>
#include <iomanip>
#include <conio.h>

static const int W = 70;
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

void MonitorView::showMonitoring(const std::string& path,
                                  const std::map<OrderStatus, int>& summary,
                                  const std::vector<StockInfo>& stocks) const {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";
    sline();
    std::cout << "  [ 모니터링 ]\n\n";

    // 주문 현황
    std::cout << "  주문 현황\n";
    sline();
    auto get = [&](OrderStatus s) -> int {
        auto it = summary.find(s);
        return it != summary.end() ? it->second : 0;
    };
    std::cout << "  " << std::left << std::setw(16) << "RESERVED"
              << std::setw(6) << get(OrderStatus::RESERVED)  << "건\n";
    std::cout << "  " << std::setw(16) << "CONFIRMED"
              << std::setw(6) << get(OrderStatus::CONFIRMED) << "건\n";
    std::cout << "  " << std::setw(16) << "PRODUCING"
              << std::setw(6) << get(OrderStatus::PRODUCING) << "건\n";
    std::cout << "  " << std::setw(16) << "RELEASE"
              << std::setw(6) << get(OrderStatus::RELEASE)   << "건\n";
    std::cout << "\n";

    // 재고 현황
    std::cout << "  재고 현황\n";
    sline();
    std::cout << "  " << padR("시료ID", 10)
              << padR("시료명",          28)
              << std::setw(8) << "재고"
              << std::setw(12) << "CONFIRMED"
              << "상태\n";
    std::cout << "  " << std::string(W, '-') << "\n";

    for (const auto& s : stocks) {
        std::string statusStr;
        switch (s.status) {
            case StockStatus::SURPLUS:  statusStr = "여유"; break;
            case StockStatus::SHORT:    statusStr = "부족"; break;
            case StockStatus::DEPLETED: statusStr = "고갈"; break;
        }
        std::cout << "  " << padR(s.sampleId, 10)
                  << padR(s.name,     28)
                  << std::left << std::setw(8)  << (std::to_string(s.stock) + " ea")
                  << std::setw(12) << (std::to_string(s.confirmedQuantity) + " ea")
                  << statusStr << "\n";
    }

    std::cout << "\n";
    sline();
    std::cout << "  (계속하려면 아무 키나 누르세요)\n선택 > ";
    _getch();
}
