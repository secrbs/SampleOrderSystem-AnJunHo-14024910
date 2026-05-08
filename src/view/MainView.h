#pragma once
#include "../model/SystemStatus.h"

class MainView {
public:
    void showMenu(const SystemStatus& status) const;
    int  getChoice() const;
    void showStub(int menuNum) const;
    void showInvalidInput() const;
};
