#include "OrderView.h"
#include <iostream>
#include <iomanip>
#include <conio.h>

static const int W = 70;
static void dline() { std::cout << "  " << std::string(W, '=') << "\n"; }
static void sline() { std::cout << "  " << std::string(W, '-') << "\n"; }

static void header(const std::string& path, const std::string& title) {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";
    sline();
    std::cout << "  [ " << title << " ]\n\n";
}

void OrderView::showMenu(const std::string& path) const {
    header(path, "시료 주문");
    std::cout << "  [1] 주문 접수\n";
    std::cout << "  [0] 뒤로\n";
    std::cout << "\n";
    sline();
    std::cout << "선택 > ";
}

int OrderView::getChoice() const {
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    if (ch >= '0' && ch <= '9') return ch - '0';
    return -1;
}

std::optional<OrderInput> OrderView::inputOrder(const std::string& path) const {
    header(path, "주문 접수");
    std::cout << "  (취소: 빈 입력 후 Enter)\n\n";

    OrderInput input;
    std::cout << "  시료 ID   > "; std::getline(std::cin, input.sampleId);
    if (input.sampleId.empty()) return std::nullopt;

    std::cout << "  고객명    > "; std::getline(std::cin, input.customerName);
    if (input.customerName.empty()) return std::nullopt;

    std::string qtyStr;
    std::cout << "  주문 수량 > "; std::getline(std::cin, qtyStr);
    if (qtyStr.empty()) return std::nullopt;
    try { input.quantity = std::stoi(qtyStr); }
    catch (...) { input.quantity = 0; }

    return input;
}

bool OrderView::confirmOrder(const std::string& path,
                              const Sample& sample,
                              const OrderInput& input) const {
    header(path, "주문 확인");
    const int L = 10;
    std::cout << "  " << std::left << std::setw(L) << "시료"
              << ": " << sample.name << " (" << sample.id << ")\n";
    std::cout << "  " << std::setw(L) << "고객"
              << ": " << input.customerName << "\n";
    std::cout << "  " << std::setw(L) << "수량"
              << ": " << input.quantity << " ea\n";
    std::cout << "\n";
    sline();
    std::cout << "  [Y] 접수    [N] 취소 > ";
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    return (ch == 'Y' || ch == 'y');
}

void OrderView::showResult(const std::string& path, const Order& order,
                            const Sample& sample) const {
    header(path, "주문 접수 완료");
    const int L = 10;
    std::cout << "  " << std::left << std::setw(L) << "주문번호"
              << ": " << order.orderId << "\n";
    std::cout << "  " << std::setw(L) << "시료"
              << ": " << sample.name << " (" << sample.id << ")\n";
    std::cout << "  " << std::setw(L) << "고객"
              << ": " << order.customerName << "\n";
    std::cout << "  " << std::setw(L) << "수량"
              << ": " << order.quantity << " ea\n";
    std::cout << "  " << std::setw(L) << "상태"
              << ": RESERVED\n";
    std::cout << "  " << std::setw(L) << "접수일시"
              << ": " << order.createdAt << "\n";
    std::cout << "\n";
    sline();
    std::cout << "  (계속하려면 아무 키나 누르세요)\n선택 > ";
    _getch();
}

void OrderView::showMessage(const std::string& msg) const {
    std::cout << "\n  " << msg << "\n";
}
