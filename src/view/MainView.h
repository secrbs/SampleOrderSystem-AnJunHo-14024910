#pragma once
#include <string>
#include "../model/SystemStatus.h"

class MainView {
public:
    void drawScreen(const SystemStatus& status) const;
    int  getChoice() const;             // 실시간 시각 갱신하며 키 대기
    void showInvalidInput() const;

private:
    void updateClock() const;           // 시각 위치만 커서 이동 후 재출력
    std::string currentTime() const;
    void moveCursor(int x, int y) const;

    // 시각이 출력되는 콘솔 좌표 (drawScreen 기준)
    static constexpr int CLOCK_ROW = 1;
    static constexpr int CLOCK_COL = 42;
};
