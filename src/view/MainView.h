#pragma once
#include <string>
#include "../model/SystemStatus.h"

class MainView {
public:
    void drawScreen(const SystemStatus& status) const;
    int  getChoice() const;
    void showInvalidInput() const;

private:
    void updateClock() const;
    std::string currentTime() const;
    void moveCursor(int x, int y) const;

    static constexpr int CLOCK_ROW = 1;
    static constexpr int CLOCK_COL = 42;
    static constexpr int INPUT_ROW = 11;  // "선택 > " 행
    static constexpr int INPUT_COL = 7;   // "선택 > " 이후 커서
};
