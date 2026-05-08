#pragma once
#include <vector>
#include <string>
#include "../model/Sample.h"

class SampleView {
public:
    void showMenu() const;
    int  getChoice() const;
    Sample inputSample() const;
    std::string inputSearchKeyword() const;
    void showSamples(const std::vector<Sample>& samples) const;
    void showSample(const Sample& sample) const;
    void showMessage(const std::string& msg) const;
};
