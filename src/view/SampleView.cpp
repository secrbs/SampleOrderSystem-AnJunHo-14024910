#include "SampleView.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <conio.h>
#include <algorithm>

static const int W = 70;

static void dline() { std::cout << "  " << std::string(W, '=') << "\n"; }
static void sline() { std::cout << "  " << std::string(W, '-') << "\n"; }

// UTF-8 문자열의 콘솔 표시 너비 계산 (한글 = 2칸, ASCII = 1칸)
static int dispWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        uint32_t cp = 0;
        int bytes = 1;
        if      (c < 0x80) { cp = c;        bytes = 1; }
        else if (c < 0xE0) { cp = c & 0x1F; bytes = 2; }
        else if (c < 0xF0) { cp = c & 0x0F; bytes = 3; }
        else               { cp = c & 0x07; bytes = 4; }
        for (int j = 1; j < bytes && i + j < s.size(); ++j)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i+j]) & 0x3F);
        i += bytes;
        // 한글(AC00-D7A3), CJK 등은 2칸
        bool wide = (cp >= 0x1100 && cp <= 0x115F) ||
                    (cp >= 0x2E80 && cp <= 0x303E) ||
                    (cp >= 0x3040 && cp <= 0xA4CF) ||
                    (cp >= 0xAC00 && cp <= 0xD7A3) ||
                    (cp >= 0xF900 && cp <= 0xFAFF) ||
                    (cp >= 0xFF01 && cp <= 0xFF60);
        w += wide ? 2 : 1;
    }
    return w;
}

// dispWidth 기준으로 오른쪽에 공백 패딩
static std::string padR(const std::string& s, int width) {
    int pad = width - dispWidth(s);
    return s + (pad > 0 ? std::string(pad, ' ') : "");
}

static void header(const std::string& path, const std::string& title) {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";
    sline();
    std::cout << "  [ " << title << " ]\n\n";
}

int SampleView::getPageSize() const {
    return 7;
}

void SampleView::drawPage(const std::string& path, const std::string& title,
                           const std::vector<Sample>& samples,
                           int page, int pageSize, int totalPages) const {
    system("cls");
    dline();
    std::cout << "  " << path << "\n";
    sline();
    std::cout << "  [ " << title << " ]"
              << "  " << (page + 1) << "/" << totalPages << " 페이지"
              << "  총 " << samples.size() << "종\n\n";

    // 컬럼 너비 (표시 너비 기준)
    const int cID    = 10;
    const int cName  = 28;
    const int cTime  = 15;
    const int cYield = 10;

    // 헤더
    std::cout << "  "
              << padR("ID",    cID)
              << padR("시료명", cName)
              << padR("생산시간", cTime)
              << padR("수율",  cYield)
              << "재고\n";
    std::cout << "  " << std::string(W, '-') << "\n";

    int start = page * pageSize;
    int end   = std::min(start + pageSize, static_cast<int>(samples.size()));
    for (int i = start; i < end; ++i) {
        const auto& s = samples[i];
        std::ostringstream tss, yss;
        tss << std::fixed << std::setprecision(2) << s.avgProductionTime << " min/ea";
        yss << std::fixed << std::setprecision(2) << s.yield;
        std::cout << "  "
                  << padR(s.id,   cID)
                  << padR(s.name, cName)
                  << padR(tss.str(), cTime)
                  << padR(yss.str(), cYield)
                  << s.stock << " ea\n";
    }

    std::cout << "\n";
    sline();
    std::cout << "  ";
    if (page > 0)              std::cout << "[P] 이전    ";
    if (page < totalPages - 1) std::cout << "[N] 다음    ";
    std::cout << "[0] 뒤로\n";
    std::cout << "선택 > ";
}

void SampleView::showMenu(const std::string& path) const {
    header(path, "시료 관리");
    std::cout << "  [1] 시료 등록\n";
    std::cout << "  [2] 시료 목록\n";
    std::cout << "  [3] 시료 검색\n";
    std::cout << "  [0] 뒤로\n";
    std::cout << "\n";
    sline();
    std::cout << "선택 > ";
}

int SampleView::getChoice() const {
    char ch = static_cast<char>(_getch());
    std::cout << ch << "\n";
    if (ch >= '0' && ch <= '9') return ch - '0';
    return -1;
}

std::optional<Sample> SampleView::inputSample() const {
    Sample s;
    std::cout << "  ID            > "; std::cin >> s.id;
    std::cin.ignore(1024, '\n');
    if (s.id.empty()) return std::nullopt;
    std::cout << "  이름          > "; std::getline(std::cin, s.name);
    if (s.name.empty()) return std::nullopt;
    std::cout << "  평균 생산시간 > "; std::cin >> s.avgProductionTime;
    std::cout << "  수율 (0~1)    > "; std::cin >> s.yield;
    std::cin.ignore(1024, '\n');
    s.stock = 0;
    return s;
}

std::string SampleView::inputSearchKeyword() const {
    std::cout << "  검색어 > ";
    std::string kw;
    std::getline(std::cin, kw);
    return kw;
}

void SampleView::showSamples(const std::string& path,
                              const std::string& title,
                              const std::vector<Sample>& samples) const {
    if (samples.empty()) {
        header(path, title);
        std::cout << "  표시할 시료가 없습니다.\n\n";
        sline();
        std::cout << "  [0] 뒤로\n선택 > ";
        _getch();
        return;
    }

    int pageSize   = getPageSize();
    int totalPages = (static_cast<int>(samples.size()) + pageSize - 1) / pageSize;
    int page       = 0;

    while (true) {
        drawPage(path, title, samples, page, pageSize, totalPages);
        char ch = static_cast<char>(_getch());
        std::cout << ch << "\n";
        if (ch == '0') break;
        if ((ch == 'N' || ch == 'n') && page < totalPages - 1) page++;
        if ((ch == 'P' || ch == 'p') && page > 0)              page--;
    }
}

void SampleView::showRegisterForm(const std::string& path) const {
    header(path, "시료 등록");
    std::cout << "  (취소: 빈 입력 후 Enter)\n\n";
}

void SampleView::showSearchForm(const std::string& path) const {
    header(path, "시료 검색");
    std::cout << "  (취소: 빈 입력 후 Enter)\n\n";
}

void SampleView::showMessage(const std::string& msg) const {
    std::cout << "\n  " << msg << "\n";
}
