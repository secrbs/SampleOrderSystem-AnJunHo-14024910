#include "ApprovalView.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <conio.h>

static const int W   = 70;
static const int BOX = 53;  // 박스 내부 표시 너비

static void dline() { std::cout << "  " << std::string(W, '=') << "\n"; }
static void sline() { std::cout << "  " << std::string(W, '-') << "\n"; }

static void header(const std::string& path, const std::string& title) {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";
    sline();
    std::cout << "  [ " << title << " ]\n\n";
}

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

// 박스 한 행: "  │  label value" → 우측 패딩 → "│"
static void boxRow(const std::string& label, const std::string& value) {
    std::string content = "  " + label + value;
    int pad = BOX - dispW(content);
    std::cout << "  │" << content << std::string(std::max(0, pad), ' ') << "│\n";
}

std::string ApprovalView::showReservedOrders(const std::string& path,
                                              const std::vector<Order>& orders) const {
    while (true) {
        header(path, "주문 승인/거절");

        if (orders.empty()) {
            std::cout << "  승인 대기 중인 주문이 없습니다.\n\n";
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

        // 잘못된 입력 → 에러 후 재루프
        std::cout << "\n  [오류] 1~" << orders.size() << " 사이의 번호를 입력하세요.\n";
        std::cout << "  (아무 키나 누르세요)\n선택 > ";
        _getch();
    }
}

char ApprovalView::getApproveOrReject(const std::string& path,
                                       const Order& order,
                                       const Sample& sample) const {
    header(path, "승인/거절 선택");

    bool sufficient = (sample.stock >= order.quantity);
    int  shortage   = std::max(0, order.quantity - sample.stock);
    int  actualProd = sufficient ? 0 :
        static_cast<int>(std::ceil(shortage / (sample.yield * 0.9)));
    double totalTime = actualProd * sample.avgProductionTime;

    const int L = 12;  // 레이블 표시 너비
    std::cout << "  " << padR("주문번호",  L) << ": " << order.orderId << "\n";
    std::cout << "  " << padR("고객",      L) << ": " << order.customerName << "\n";
    std::cout << "  " << padR("시료",      L) << ": " << sample.name << " (" << sample.id << ")\n";
    std::cout << "  " << padR("주문 수량", L) << ": " << order.quantity << " ea\n";
    std::cout << "\n";
    std::cout << "  " << padR("현재 재고", L) << ": " << sample.stock << " ea\n";
    if (!sufficient) {
        std::ostringstream tss;
        tss << std::fixed << std::setprecision(1) << totalTime;
        std::cout << "  " << padR("부족분",    L) << ": " << shortage    << " ea\n";
        std::cout << "  " << padR("실생산량",  L) << ": " << actualProd  << " ea\n";
        std::cout << "  " << padR("총 생산시간", L) << ": " << tss.str() << " min\n";
    }
    std::cout << "\n";
    sline();
    std::cout << "  [Y] 승인    [N] 거절    [0] 취소\n선택 > ";
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    return ch;
}

void ApprovalView::showResult(const std::string& path, const Order& order,
                               int remainingStock) const {
    header(path, "처리 완료");

    std::string statusStr;
    switch (order.status) {
        case OrderStatus::CONFIRMED: statusStr = "CONFIRMED"; break;
        case OrderStatus::PRODUCING: statusStr = "PRODUCING"; break;
        case OrderStatus::REJECTED:  statusStr = "REJECTED";  break;
        default:                     statusStr = "알 수 없음";
    }

    const int L = 12;
    std::cout << "  " << padR("주문번호",  L) << ": " << order.orderId << "\n";
    std::cout << "  " << padR("처리 결과", L) << ": " << statusStr << "\n";
    if (order.status == OrderStatus::CONFIRMED && remainingStock >= 0)
        std::cout << "  " << padR("남은 재고", L) << ": " << remainingStock << " ea\n";
    std::cout << "\n";
    sline();
    std::cout << "  (계속하려면 아무 키나 누르세요)\n선택 > ";
    _getch();
}

void ApprovalView::showMessage(const std::string& msg) const {
    std::cout << "\n  " << msg << "\n";
}
