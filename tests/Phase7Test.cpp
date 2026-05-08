#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/ShipmentController.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase7Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(orderPath, R"([
            {"orderId":"ORD-001","sampleId":"S-001","customerName":"고객A","quantity":50,
             "status":"CONFIRMED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-002","sampleId":"S-001","customerName":"고객B","quantity":30,
             "status":"RESERVED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-003","sampleId":"S-001","customerName":"고객C","quantity":20,
             "status":"CONFIRMED","createdAt":"","updatedAt":""}
        ])");
        writeJson(queuePath, "[]");
    }
    void TearDown() override {
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string orderPath = "test_orders_p7.json";
    const std::string queuePath = "test_queue_p7.json";
};

// T7-01: CONFIRMED 주문 출고 실행 → RELEASE
TEST_F(Phase7Test, T7_01_ReleaseConfirmed) {
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(or_);

    EXPECT_TRUE(ctrl.release("ORD-001"));
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o) { return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::RELEASE);
}

// T7-02: CONFIRMED 아닌 주문 출고 시도 → false
TEST_F(Phase7Test, T7_02_ReleaseNonConfirmed) {
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(or_);

    EXPECT_FALSE(ctrl.release("ORD-002"));  // RESERVED
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o) { return o.orderId == "ORD-002"; });
    EXPECT_EQ(it->status, OrderStatus::RESERVED);  // 변경 없음
}

// T7-03: CONFIRMED 주문 목록 조회
TEST_F(Phase7Test, T7_03_GetConfirmedOrders) {
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(or_);

    auto confirmed = ctrl.getConfirmedOrders();
    EXPECT_EQ(confirmed.size(), 2u);  // ORD-001, ORD-003
    for (const auto& o : confirmed)
        EXPECT_EQ(o.status, OrderStatus::CONFIRMED);
}

// T7-04: 출고 후 RELEASE 상태 영속성 확인
TEST_F(Phase7Test, T7_04_ReleasePersistence) {
    {
        OrderRepository    or_(orderPath, queuePath);
        ShipmentController ctrl(or_);
        ctrl.release("ORD-001");
    }
    OrderRepository reloaded(orderPath, queuePath);
    auto orders = reloaded.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o) { return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::RELEASE);
}

// T7-05: 존재하지 않는 주문 출고 시도 → false
TEST_F(Phase7Test, T7_05_ReleaseNotExist) {
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(or_);

    EXPECT_FALSE(ctrl.release("ORD-999"));
}
