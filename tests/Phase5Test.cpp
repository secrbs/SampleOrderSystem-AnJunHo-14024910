#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/MonitoringController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase5Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(samplePath, R"([
            {"id":"S-001","name":"웨이퍼A","avgProductionTime":0.5,"yield":0.9,"stock":100},
            {"id":"S-002","name":"웨이퍼B","avgProductionTime":0.3,"yield":0.8,"stock":0},
            {"id":"S-003","name":"웨이퍼C","avgProductionTime":0.8,"yield":0.9,"stock":30}
        ])");
        writeJson(orderPath, R"([
            {"orderId":"ORD-001","sampleId":"S-001","customerName":"A","quantity":50,
             "status":"RESERVED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-002","sampleId":"S-001","customerName":"B","quantity":80,
             "status":"CONFIRMED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-003","sampleId":"S-002","customerName":"C","quantity":30,
             "status":"PRODUCING","createdAt":"","updatedAt":""},
            {"orderId":"ORD-004","sampleId":"S-003","customerName":"D","quantity":50,
             "status":"CONFIRMED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-005","sampleId":"S-001","customerName":"E","quantity":10,
             "status":"RELEASE","createdAt":"","updatedAt":""},
            {"orderId":"ORD-006","sampleId":"S-001","customerName":"F","quantity":5,
             "status":"REJECTED","createdAt":"","updatedAt":""}
        ])");
        writeJson(queuePath, "[]");
    }
    void TearDown() override {
        std::remove(samplePath.c_str());
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string samplePath = "test_samples_p5.json";
    const std::string orderPath  = "test_orders_p5.json";
    const std::string queuePath  = "test_queue_p5.json";
};

// T5-01: 상태별 주문 수 집계 (REJECTED 제외)
TEST_F(Phase5Test, T5_01_OrderCountByStatus) {
    SampleRepository     sr(samplePath);
    OrderRepository      or_(orderPath, queuePath);
    MonitoringController ctrl(sr, or_);

    auto summary = ctrl.getOrderSummary();
    EXPECT_EQ(summary.at(OrderStatus::RESERVED),  1);
    EXPECT_EQ(summary.at(OrderStatus::CONFIRMED),  2);
    EXPECT_EQ(summary.at(OrderStatus::PRODUCING),  1);
    EXPECT_EQ(summary.at(OrderStatus::RELEASE),    1);
}

// T5-02: REJECTED는 집계에 포함되지 않음
TEST_F(Phase5Test, T5_02_RejectedExcluded) {
    SampleRepository     sr(samplePath);
    OrderRepository      or_(orderPath, queuePath);
    MonitoringController ctrl(sr, or_);

    auto summary = ctrl.getOrderSummary();
    EXPECT_EQ(summary.count(OrderStatus::REJECTED), 0u);
}

// T5-03: 재고 0 → DEPLETED
TEST_F(Phase5Test, T5_03_StockDepleted) {
    SampleRepository     sr(samplePath);
    OrderRepository      or_(orderPath, queuePath);
    MonitoringController ctrl(sr, or_);

    auto stocks = ctrl.getStockInfo();
    auto it = std::find_if(stocks.begin(), stocks.end(),
        [](const StockInfo& s){ return s.sampleId == "S-002"; });
    EXPECT_EQ(it->status, StockStatus::DEPLETED);
}

// T5-04: 재고 < CONFIRMED 합계 → SHORT
TEST_F(Phase5Test, T5_04_StockShort) {
    SampleRepository     sr(samplePath);
    OrderRepository      or_(orderPath, queuePath);
    MonitoringController ctrl(sr, or_);

    // S-003: 재고 30, CONFIRMED 수량 50 → 부족
    auto stocks = ctrl.getStockInfo();
    auto it = std::find_if(stocks.begin(), stocks.end(),
        [](const StockInfo& s){ return s.sampleId == "S-003"; });
    EXPECT_EQ(it->status, StockStatus::SHORT);
}

// T5-05: 재고 ≥ CONFIRMED 합계 → SURPLUS
TEST_F(Phase5Test, T5_05_StockSurplus) {
    SampleRepository     sr(samplePath);
    OrderRepository      or_(orderPath, queuePath);
    MonitoringController ctrl(sr, or_);

    // S-001: 재고 100, CONFIRMED 수량 80 → 여유
    auto stocks = ctrl.getStockInfo();
    auto it = std::find_if(stocks.begin(), stocks.end(),
        [](const StockInfo& s){ return s.sampleId == "S-001"; });
    EXPECT_EQ(it->status, StockStatus::SURPLUS);
}
