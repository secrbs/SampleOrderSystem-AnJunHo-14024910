#include "OrderView.h"
#include <iostream>
#include <iomanip>
#include <conio.h>

void OrderView::showMenu() const {
    std::cout << "\n[ 시료 주문 ]\n";
    std::cout << "  [1] 주문 접수\n";
    std::cout << "  [0] 뒤로\n";
    std::cout << "선택 > ";
}

int OrderView::getChoice() const {
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    if (ch >= '0' && ch <= '9') return ch - '0';
    return -1;
}

void OrderView::inputOrderInfo(std::string& sampleId,
                                std::string& customerName,
                                int& quantity) const {
    std::cout << "  시료 ID  > "; std::cin >> sampleId;
    std::cin.ignore(1024, '\n');
    std::cout << "  고객명   > "; std::getline(std::cin, customerName);
    std::cout << "  주문 수량 > "; std::cin >> quantity;
    std::cin.ignore(1024, '\n');
}

bool OrderView::confirmOrder(const Sample& sample,
                              const std::string& customerName,
                              int quantity) const {
    std::cout << "\n  [ 입력 내용 확인 ]\n";
    std::cout << "  시료   : " << sample.name << " (" << sample.id << ")\n";
    std::cout << "  고객   : " << customerName << "\n";
    std::cout << "  수량   : " << quantity << " ea\n";
    std::cout << "  [Y] 접수  [N] 취소 > ";
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    return (ch == 'Y' || ch == 'y');
}

void OrderView::showOrderResult(const Order& order) const {
    std::cout << "\n  주문 접수 완료.\n";
    std::cout << "  주문번호 : " << order.orderId << "\n";
    std::cout << "  현재 상태: RESERVED\n";
}

void OrderView::showMessage(const std::string& msg) const {
    std::cout << "  " << msg << "\n";
}
