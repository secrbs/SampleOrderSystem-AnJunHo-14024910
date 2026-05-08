#include "SampleView.h"
#include <iostream>
#include <iomanip>
#include <conio.h>

void SampleView::showMenu() const {
    std::cout << "\n[ 시료 관리 ]\n";
    std::cout << "  [1] 시료 등록\n";
    std::cout << "  [2] 시료 목록\n";
    std::cout << "  [3] 시료 검색\n";
    std::cout << "  [0] 뒤로\n";
    std::cout << "선택 > ";
}

int SampleView::getChoice() const {
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    if (ch >= '0' && ch <= '9') return ch - '0';
    return -1;
}

Sample SampleView::inputSample() const {
    Sample s;
    std::cout << "  ID             > "; std::cin >> s.id;
    std::cin.ignore(1024, '\n');
    std::cout << "  이름           > "; std::getline(std::cin, s.name);
    std::cout << "  평균 생산시간  > "; std::cin >> s.avgProductionTime;
    std::cout << "  수율 (0~1)     > "; std::cin >> s.yield;
    std::cin.ignore(1024, '\n');
    s.stock = 0;
    return s;
}

std::string SampleView::inputSearchKeyword() const {
    std::cout << "  검색어 > ";
    std::cin.ignore(1024, '\n');
    std::string kw;
    std::getline(std::cin, kw);
    return kw;
}

void SampleView::showSamples(const std::vector<Sample>& samples) const {
    if (samples.empty()) {
        std::cout << "  등록된 시료가 없습니다.\n";
        return;
    }
    std::cout << "\n  " << std::left
              << std::setw(8)  << "ID"
              << std::setw(22) << "시료명"
              << std::setw(14) << "생산시간"
              << std::setw(8)  << "수율"
              << std::setw(8)  << "재고"
              << "\n";
    std::cout << "  " << std::string(60, '-') << "\n";
    for (const auto& s : samples) {
        std::cout << "  " << std::left
                  << std::setw(8)  << s.id
                  << std::setw(22) << s.name
                  << std::setw(14) << (std::to_string(s.avgProductionTime) + " min/ea")
                  << std::setw(8)  << s.yield
                  << std::setw(8)  << s.stock
                  << "\n";
    }
    std::cout << "  총 " << samples.size() << "종\n";
}

void SampleView::showSample(const Sample& s) const {
    std::cout << "  ID: " << s.id << "  이름: " << s.name
              << "  수율: " << s.yield << "  재고: " << s.stock << "\n";
}

void SampleView::showMessage(const std::string& msg) const {
    std::cout << "  " << msg << "\n";
}
