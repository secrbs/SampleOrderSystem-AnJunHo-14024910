#pragma once
#include <string>
#include <vector>
#include <optional>
#include "../model/Sample.h"

class SampleView {
public:
    void showMenu(const std::string& path) const;
    int  getChoice() const;

    void showRegisterForm(const std::string& path) const;
    std::optional<Sample> inputSample() const;   // nullopt = 취소

    void showSearchForm(const std::string& path) const;
    std::string inputSearchKeyword() const;       // 빈 문자열 = 취소

    void showSamples(const std::string& path,
                     const std::string& title,
                     const std::vector<Sample>& samples) const;

    void showMessage(const std::string& msg) const;

private:
    int  getPageSize() const;
    void drawPage(const std::string& path, const std::string& title,
                  const std::vector<Sample>& samples,
                  int page, int pageSize, int totalPages) const;
};
