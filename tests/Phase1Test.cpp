#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/MainController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase1Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(samplePath, "[]");
        writeJson(orderPath,  "[]");
        writeJson(queuePath,  "[]");
    }
    void TearDown() override {
        std::remove(samplePath.c_str());
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string samplePath = "test_samples_p1.json";
    const std::string orderPath  = "test_orders_p1.json";
    const std::string queuePath  = "test_queue_p1.json";
};

// T1-01: 메뉴 0 입력 → false (종료)
TEST_F(Phase1Test, T1_01_MenuExit) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    MainController   ctrl(sr, or_);
    EXPECT_FALSE(ctrl.handleMenu(0));
}

// T1-02: 메뉴 1~8 입력 → true (계속)
TEST_F(Phase1Test, T1_02_MenuValid) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    MainController   ctrl(sr, or_);
    for (int i = 1; i <= 8; ++i)
        EXPECT_TRUE(ctrl.handleMenu(i)) << "menu=" << i;
}

// T1-03: 잘못된 입력 → true (오류 처리 후 계속)
TEST_F(Phase1Test, T1_03_MenuInvalid) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    MainController   ctrl(sr, or_);
    EXPECT_TRUE(ctrl.handleMenu(-1));
    EXPECT_TRUE(ctrl.handleMenu(9));
    EXPECT_TRUE(ctrl.handleMenu(99));
}

// T1-04: 시스템 현황 — 빈 데이터
TEST_F(Phase1Test, T1_04_SystemStatusEmpty) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    MainController   ctrl(sr, or_);
    auto st = ctrl.getSystemStatus();
    EXPECT_EQ(st.sampleCount,          0);
    EXPECT_EQ(st.totalStock,           0);
    EXPECT_EQ(st.orderCount,           0);
    EXPECT_EQ(st.productionQueueCount, 0);
}

// T1-05: 시스템 현황 — 실제 데이터
TEST_F(Phase1Test, T1_05_SystemStatusWithData) {
    writeJson(samplePath, R"([
        {"id":"S-001","name":"웨이퍼A","avgProductionTime":0.5,"yield":0.9,"stock":100},
        {"id":"S-002","name":"웨이퍼B","avgProductionTime":0.3,"yield":0.8,"stock":50}
    ])");
    writeJson(orderPath, R"([
        {"orderId":"ORD-001","sampleId":"S-001","customerName":"고객A",
         "quantity":10,"status":"RESERVED","createdAt":"","updatedAt":""},
        {"orderId":"ORD-002","sampleId":"S-002","customerName":"고객B",
         "quantity":20,"status":"CONFIRMED","createdAt":"","updatedAt":""}
    ])");
    writeJson(queuePath, R"([
        {"orderId":"ORD-003","sampleId":"S-001","shortage":50,
         "actualProduction":61,"totalTime":30.5,"enqueuedAt":""}
    ])");

    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    MainController   ctrl(sr, or_);
    auto st = ctrl.getSystemStatus();
    EXPECT_EQ(st.sampleCount,          2);
    EXPECT_EQ(st.totalStock,           150);
    EXPECT_EQ(st.orderCount,           2);
    EXPECT_EQ(st.productionQueueCount, 1);
}
