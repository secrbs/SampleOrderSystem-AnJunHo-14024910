#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/OrderController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase3Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(samplePath, R"([{"id":"S-001","name":"웨이퍼A","avgProductionTime":0.5,"yield":0.9,"stock":100}])");
        writeJson(orderPath,  "[]");
        writeJson(queuePath,  "[]");
    }
    void TearDown() override {
        std::remove(samplePath.c_str());
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string samplePath = "test_samples_p3.json";
    const std::string orderPath  = "test_orders_p3.json";
    const std::string queuePath  = "test_queue_p3.json";
};

// T3-01: 유효한 주문 접수 → RESERVED 상태 Order 생성
TEST_F(Phase3Test, T3_01_PlaceOrderValid) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    OrderController  ctrl(sr, or_);

    auto result = ctrl.placeOrder("S-001", "고객A", 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->status, OrderStatus::RESERVED);
    EXPECT_EQ(result->sampleId, "S-001");
    EXPECT_EQ(result->customerName, "고객A");
    EXPECT_EQ(result->quantity, 10);
    EXPECT_EQ(or_.count(), 1);
}

// T3-02: 존재하지 않는 시료 ID → nullopt
TEST_F(Phase3Test, T3_02_PlaceOrderInvalidSampleId) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    OrderController  ctrl(sr, or_);

    auto result = ctrl.placeOrder("S-999", "고객A", 10);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(or_.count(), 0);
}

// T3-03: 주문 수량 0 이하 → nullopt
TEST_F(Phase3Test, T3_03_PlaceOrderInvalidQuantity) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    OrderController  ctrl(sr, or_);

    EXPECT_FALSE(ctrl.placeOrder("S-001", "고객A",  0).has_value());
    EXPECT_FALSE(ctrl.placeOrder("S-001", "고객A", -1).has_value());
    EXPECT_EQ(or_.count(), 0);
}

// T3-04: 같은 날 2건 주문 → 순번 -0001, -0002
TEST_F(Phase3Test, T3_04_SequentialOrderId) {
    SampleRepository sr(samplePath);
    OrderRepository  or_(orderPath, queuePath);
    OrderController  ctrl(sr, or_);

    auto o1 = ctrl.placeOrder("S-001", "고객A", 10);
    auto o2 = ctrl.placeOrder("S-001", "고객B", 20);
    ASSERT_TRUE(o1.has_value());
    ASSERT_TRUE(o2.has_value());
    EXPECT_NE(o1->orderId, o2->orderId);
    EXPECT_TRUE(o1->orderId.find("0001") != std::string::npos);
    EXPECT_TRUE(o2->orderId.find("0002") != std::string::npos);
}

// T3-05: JSON 저장 후 재로드
TEST_F(Phase3Test, T3_05_PersistAndReload) {
    {
        SampleRepository sr(samplePath);
        OrderRepository  or_(orderPath, queuePath);
        OrderController  ctrl(sr, or_);
        ctrl.placeOrder("S-001", "고객A", 10);
    }
    OrderRepository reloaded(orderPath, queuePath);
    EXPECT_EQ(reloaded.count(), 1);
    EXPECT_EQ(reloaded.findAll()[0].status, OrderStatus::RESERVED);
}
