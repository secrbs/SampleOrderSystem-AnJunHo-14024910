#pragma once
#include <vector>
#include <optional>
#include <string>
#include "../model/Order.h"

class ProductionView {
public:
    void showStatus(const std::optional<ProductionJob>& current,
                    const std::vector<ProductionJob>& queue,
                    double elapsedMinutes) const;
    void showMessage(const std::string& msg) const;
};
