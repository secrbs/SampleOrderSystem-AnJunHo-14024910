#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/ApprovalController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase4Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(samplePath, R"([
            {"id":"S-001","name":"웨이퍼A","avgProductionTime":0.5,"yield":0.9,"stock":100}
        ])");
        writeJson(orderPath, R"([
            {"orderId":"ORD-001","sampleId":"S-001","customerName":"고객A",
             "quantity":50,"status":"RESERVED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-002","sampleId":"S-001","customerName":"고객B",
             "quantity":200,"status":"RESERVED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-003","sampleId":"S-001","customerName":"고객C",
             "quantity":100,"status":"RESERVED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-004","sampleId":"S-001","customerName":"고객D",
             "quantity":10,"status":"CONFIRMED","createdAt":"","updatedAt":""}
        ])");
        writeJson(queuePath, "[]");
    }
    void TearDown() override {
        std::remove(samplePath.c_str());
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string samplePath = "test_samples_p4.json";
    const std::string orderPath  = "test_orders_p4.json";
    const std::string queuePath  = "test_queue_p4.json";
};

// T4-01: 재고 충분 → CONFIRMED (재고 차감은 출고 시)
TEST_F(Phase4Test, T4_01_ApproveWithSufficientStock) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ApprovalController ctrl(sr, or_);

    EXPECT_TRUE(ctrl.approve("ORD-001"));  // 수량 50, 재고 100 → 충분
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::CONFIRMED);
    EXPECT_EQ(sr.findById("S-001")->stock, 100);  // 재고 차감 없음
}

// T4-02: 재고 부족 → PRODUCING, 큐 등록
TEST_F(Phase4Test, T4_02_ApproveWithInsufficientStock) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ApprovalController ctrl(sr, or_);

    EXPECT_TRUE(ctrl.approve("ORD-002"));  // 수량 200, 재고 100 → 부족
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-002"; });
    EXPECT_EQ(it->status, OrderStatus::PRODUCING);
    EXPECT_EQ(or_.productionQueueCount(), 1);
}

// T4-03: 재고 정확히 일치 → CONFIRMED
TEST_F(Phase4Test, T4_03_ApproveExactStock) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ApprovalController ctrl(sr, or_);

    EXPECT_TRUE(ctrl.approve("ORD-003"));  // 수량 100, 재고 100 → 충분
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-003"; });
    EXPECT_EQ(it->status, OrderStatus::CONFIRMED);
    EXPECT_EQ(sr.findById("S-001")->stock, 100);  // 재고 차감 없음
}

// T4-04: 거절 → REJECTED, 재고 변화 없음
TEST_F(Phase4Test, T4_04_Reject) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ApprovalController ctrl(sr, or_);

    EXPECT_TRUE(ctrl.reject("ORD-001"));
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::REJECTED);
    EXPECT_EQ(sr.findById("S-001")->stock, 100);  // 변화 없음
}

// T4-05: RESERVED 아닌 주문 승인 시도 → false
TEST_F(Phase4Test, T4_05_ApproveNonReserved) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ApprovalController ctrl(sr, or_);

    EXPECT_FALSE(ctrl.approve("ORD-004"));  // CONFIRMED 상태
}

// T4-06: RESERVED 목록 조회
TEST_F(Phase4Test, T4_06_GetReservedOrders) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ApprovalController ctrl(sr, or_);

    auto list = ctrl.getReservedOrders();
    EXPECT_EQ(list.size(), 3u);
    for (const auto& o : list)
        EXPECT_EQ(o.status, OrderStatus::RESERVED);
}
