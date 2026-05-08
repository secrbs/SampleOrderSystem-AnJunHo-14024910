#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/ApprovalController.h"
#include "controller/ProductionController.h"
#include "controller/MonitoringController.h"
#include "controller/ShipmentController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase8Test : public ::testing::Test {
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

    const std::string samplePath = "test_s_p8.json";
    const std::string orderPath  = "test_o_p8.json";
    const std::string queuePath  = "test_q_p8.json";
};

// T8-01: 재고 충분 전체 흐름 — 시료 등록 → 주문 → 승인 → 출고
TEST_F(Phase8Test, T8_01_FullFlow_SufficientStock) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);

    SampleController    sampleCtrl(sr);
    OrderController     orderCtrl(sr, or_);
    ApprovalController  approvalCtrl(sr, or_);
    ShipmentController  shipCtrl(or_);

    // 시료 등록
    ASSERT_TRUE(sampleCtrl.registerSample({"S-001", "웨이퍼A", 0.5, 0.9, 100}));

    // 주문 접수
    auto order = orderCtrl.placeOrder("S-001", "고객A", 50);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->status, OrderStatus::RESERVED);

    // 승인 (재고 충분 → CONFIRMED)
    EXPECT_TRUE(approvalCtrl.approve(order->orderId));
    EXPECT_EQ(sr.findById("S-001")->stock, 50);

    // 출고
    EXPECT_TRUE(shipCtrl.release(order->orderId));
    auto orders = or_.findAll();
    EXPECT_EQ(orders[0].status, OrderStatus::RELEASE);
}

// T8-02: 재고 부족 전체 흐름 — 주문 → 승인(PRODUCING) → 생산 완료 → 출고
TEST_F(Phase8Test, T8_02_FullFlow_InsufficientStock) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);

    SampleController    sampleCtrl(sr);
    OrderController     orderCtrl(sr, or_);
    ApprovalController  approvalCtrl(sr, or_);
    ProductionController prodCtrl(sr, or_);
    ShipmentController  shipCtrl(or_);

    // 시료 등록 (재고 10, 주문 100 → 부족)
    ASSERT_TRUE(sampleCtrl.registerSample({"S-001", "웨이퍼A", 0.5, 0.9, 10}));

    // 주문
    auto order = orderCtrl.placeOrder("S-001", "고객A", 100);
    ASSERT_TRUE(order.has_value());

    // 승인 → PRODUCING, 큐 등록
    EXPECT_TRUE(approvalCtrl.approve(order->orderId));
    auto orders = or_.findAll();
    EXPECT_EQ(orders[0].status, OrderStatus::PRODUCING);
    EXPECT_EQ(or_.productionQueueCount(), 1);

    // 생산 완료 → 재고 증가, CONFIRMED
    EXPECT_TRUE(prodCtrl.completeCurrentJob());
    orders = or_.findAll();
    EXPECT_EQ(orders[0].status, OrderStatus::CONFIRMED);
    EXPECT_GT(sr.findById("S-001")->stock, 10);

    // 출고
    EXPECT_TRUE(shipCtrl.release(order->orderId));
    orders = or_.findAll();
    EXPECT_EQ(orders[0].status, OrderStatus::RELEASE);
}

// T8-03: 모니터링 — 전체 흐름 후 현황 집계
TEST_F(Phase8Test, T8_03_MonitoringAfterFullFlow) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);

    SampleController    sampleCtrl(sr);
    OrderController     orderCtrl(sr, or_);
    ApprovalController  approvalCtrl(sr, or_);
    MonitoringController monCtrl(sr, or_);

    ASSERT_TRUE(sampleCtrl.registerSample({"S-001", "웨이퍼A", 0.5, 0.9, 200}));

    auto o1 = orderCtrl.placeOrder("S-001", "고객A", 50);
    auto o2 = orderCtrl.placeOrder("S-001", "고객B", 30);
    approvalCtrl.approve(o1->orderId);   // CONFIRMED
    approvalCtrl.reject(o2->orderId);    // REJECTED

    auto summary = monCtrl.getOrderSummary();
    EXPECT_EQ(summary[OrderStatus::CONFIRMED], 1);
    EXPECT_EQ(summary.count(OrderStatus::REJECTED), 0u);  // REJECTED 제외

    auto stocks = monCtrl.getStockInfo();
    EXPECT_EQ(stocks[0].status, StockStatus::SURPLUS);  // 재고 150 > CONFIRMED 50
}
