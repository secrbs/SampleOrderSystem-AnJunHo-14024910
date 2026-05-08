#include "ShipmentView.h"
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

std::string ShipmentView::showConfirmedOrders(const std::string& path,
                                               const std::vector<Order>& orders) const {
    while (true) {
        system("cls");
        dline();
        std::cout << "  " << path << "\n";
        sline();
        std::cout << "  [ 출고 처리 ]\n\n";

        if (orders.empty()) {
            std::cout << "  출고 가능한 주문이 없습니다.\n\n";
            sline();
            std::cout << "  (아무 키나 누르세요)\n선택 > ";
            _getch();
            return "";
        }

        std::cout << "  " << std::left
                  << std::setw(5)  << "No."
                  << padR("주문번호",    20)
                  << padR("고객",        16)
                  << padR("시료ID",      10)
                  << "수량\n";
        std::cout << "  " << std::string(W, '-') << "\n";

        for (size_t i = 0; i < orders.size(); ++i) {
            const auto& o = orders[i];
            std::cout << "  "
                      << std::left << std::setw(5) << ("[" + std::to_string(i + 1) + "]")
                      << padR(o.orderId,      20)
                      << padR(o.customerName, 16)
                      << padR(o.sampleId,     10)
                      << o.quantity << " ea\n";
        }

        std::cout << "\n";
        sline();
        std::cout << "  번호 입력 (취소: 빈 입력 후 Enter)\n선택 > ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) return "";

        try {
            int idx = std::stoi(input);
            if (idx >= 1 && idx <= static_cast<int>(orders.size()))
                return orders[idx - 1].orderId;
        } catch (...) {}

        std::cout << "\n  [오류] 1~" << orders.size() << " 사이의 번호를 입력하세요.\n";
        std::cout << "  (아무 키나 누르세요)\n선택 > ";
        _getch();
    }
}

void ShipmentView::showResult(const std::string& path, const Order& order,
                               int remainingStock) const {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";
    sline();
    std::cout << "  [ 출고 처리 완료 ]\n\n";

    const int L = 12;
    std::cout << "  " << padR("주문번호",  L) << ": " << order.orderId << "\n";
    std::cout << "  " << padR("고객",      L) << ": " << order.customerName << "\n";
    std::cout << "  " << padR("시료",      L) << ": " << order.sampleId << "\n";
    std::cout << "  " << padR("출고 수량", L) << ": " << order.quantity << " ea\n";
    std::cout << "  " << padR("처리 결과", L) << ": RELEASE\n";
    if (remainingStock >= 0)
        std::cout << "  " << padR("남은 재고", L) << ": " << remainingStock << " ea\n";
    std::cout << "\n";
    sline();
    std::cout << "  (계속하려면 아무 키나 누르세요)\n선택 > ";
    _getch();
}

void ShipmentView::showMessage(const std::string& msg) const {
    std::cout << "\n  " << msg << "\n";
}
