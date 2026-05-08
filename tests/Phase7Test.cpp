#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/ShipmentController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase7Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(samplePath, R"([
            {"id":"S-001","name":"웨이퍼A","avgProductionTime":0.5,"yield":0.9,"stock":200}
        ])");
        writeJson(orderPath, R"([
            {"orderId":"ORD-001","sampleId":"S-001","customerName":"고객A",
             "quantity":50,"status":"CONFIRMED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-002","sampleId":"S-001","customerName":"고객B",
             "quantity":30,"status":"RESERVED","createdAt":"","updatedAt":""},
            {"orderId":"ORD-003","sampleId":"S-001","customerName":"고객C",
             "quantity":20,"status":"CONFIRMED","createdAt":"","updatedAt":""}
        ])");
        writeJson(queuePath, "[]");
    }
    void TearDown() override {
        std::remove(samplePath.c_str());
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string samplePath = "test_samples_p7.json";
    const std::string orderPath  = "test_orders_p7.json";
    const std::string queuePath  = "test_queue_p7.json";
};

// T7-01: CONFIRMED → RELEASE, 재고 차감
TEST_F(Phase7Test, T7_01_ReleaseConfirmed) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(sr, or_);

    EXPECT_TRUE(ctrl.release("ORD-001"));
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::RELEASE);
    EXPECT_EQ(sr.findById("S-001")->stock, 150);  // 200 - 50
}

// T7-02: CONFIRMED 아닌 주문 출고 시도 → false
TEST_F(Phase7Test, T7_02_ReleaseNonConfirmed) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(sr, or_);

    EXPECT_FALSE(ctrl.release("ORD-002"));  // RESERVED
    EXPECT_EQ(sr.findById("S-001")->stock, 200);  // 재고 변화 없음
}

// T7-03: 존재하지 않는 주문 → false
TEST_F(Phase7Test, T7_03_ReleaseNotExist) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(sr, or_);

    EXPECT_FALSE(ctrl.release("ORD-999"));
}

// T7-04: CONFIRMED 목록 조회
TEST_F(Phase7Test, T7_04_GetConfirmedOrders) {
    SampleRepository   sr(samplePath);
    OrderRepository    or_(orderPath, queuePath);
    ShipmentController ctrl(sr, or_);

    auto list = ctrl.getConfirmedOrders();
    EXPECT_EQ(list.size(), 2u);
    for (const auto& o : list)
        EXPECT_EQ(o.status, OrderStatus::CONFIRMED);
}

// T7-05: JSON 저장 후 재로드
TEST_F(Phase7Test, T7_05_PersistAndReload) {
    {
        SampleRepository   sr(samplePath);
        OrderRepository    or_(orderPath, queuePath);
        ShipmentController ctrl(sr, or_);
        ctrl.release("ORD-001");
    }
    OrderRepository reloaded(orderPath, queuePath);
    auto orders = reloaded.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::RELEASE);
}
