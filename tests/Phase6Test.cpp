#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/ProductionController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase6Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(samplePath, R"([
            {"id":"S-001","name":"웨이퍼A","avgProductionTime":0.8,"yield":0.92,"stock":30}
        ])");
        writeJson(orderPath, R"([
            {"orderId":"ORD-001","sampleId":"S-001","customerName":"A","quantity":200,
             "status":"PRODUCING","createdAt":"","updatedAt":""},
            {"orderId":"ORD-002","sampleId":"S-001","customerName":"B","quantity":100,
             "status":"PRODUCING","createdAt":"","updatedAt":""},
            {"orderId":"ORD-003","sampleId":"S-001","customerName":"C","quantity":50,
             "status":"PRODUCING","createdAt":"","updatedAt":""}
        ])");
        writeJson(queuePath, R"([
            {"orderId":"ORD-001","sampleId":"S-001","shortage":170,"actualProduction":206,"totalTime":164.8,"enqueuedAt":""},
            {"orderId":"ORD-002","sampleId":"S-001","shortage":70,"actualProduction":85,"totalTime":68.0,"enqueuedAt":""},
            {"orderId":"ORD-003","sampleId":"S-001","shortage":20,"actualProduction":25,"totalTime":20.0,"enqueuedAt":""}
        ])");
    }
    void TearDown() override {
        std::remove(samplePath.c_str());
        std::remove(orderPath.c_str());
        std::remove(queuePath.c_str());
    }

    const std::string samplePath = "test_samples_p6.json";
    const std::string orderPath  = "test_orders_p6.json";
    const std::string queuePath  = "test_queue_p6.json";
};

// T6-01: 실생산량 계산 (부족분 170, 수율 0.92)
TEST_F(Phase6Test, T6_01_CalcActualProduction) {
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    EXPECT_EQ(ctrl.calcActualProduction(170, 0.92), 206);
}

// T6-02: FIFO 순서 보장
TEST_F(Phase6Test, T6_02_FifoOrder) {
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    auto queue = ctrl.getQueue();
    ASSERT_EQ(queue.size(), 3u);
    EXPECT_EQ(queue[0].orderId, "ORD-001");
    EXPECT_EQ(queue[1].orderId, "ORD-002");
    EXPECT_EQ(queue[2].orderId, "ORD-003");
}

// T6-03: 빈 큐 상태 조회
TEST_F(Phase6Test, T6_03_EmptyQueue) {
    writeJson(queuePath, "[]");
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    EXPECT_TRUE(ctrl.getQueue().empty());
    EXPECT_FALSE(ctrl.getCurrentJob().has_value());
}

// T6-04: 생산 완료 처리 → 재고 증가, CONFIRMED
TEST_F(Phase6Test, T6_04_CompleteProduction) {
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    EXPECT_TRUE(ctrl.completeCurrentJob());
    auto orders = or_.findAll();
    auto it = std::find_if(orders.begin(), orders.end(),
        [](const Order& o){ return o.orderId == "ORD-001"; });
    EXPECT_EQ(it->status, OrderStatus::CONFIRMED);
    EXPECT_EQ(sr.findById("S-001")->stock, 30 + 206);  // 재고 증가
}

// T6-05: 생산 완료 후 다음 작업 자동 전환
TEST_F(Phase6Test, T6_05_NextJobAfterComplete) {
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    ctrl.completeCurrentJob();
    auto queue = ctrl.getQueue();
    ASSERT_EQ(queue.size(), 2u);
    EXPECT_EQ(queue[0].orderId, "ORD-002");
}

// T6-06: 총생산시간 계산
TEST_F(Phase6Test, T6_06_CalcTotalTime) {
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    EXPECT_DOUBLE_EQ(ctrl.calcTotalTime(0.8, 206), 164.8);
}

// T6-07: 큐 영속성
TEST_F(Phase6Test, T6_07_QueuePersistence) {
    {
        SampleRepository    sr(samplePath);
        OrderRepository     or_(orderPath, queuePath);
        ProductionController ctrl(sr, or_);
        ctrl.completeCurrentJob();
    }
    OrderRepository reloaded(orderPath, queuePath);
    EXPECT_EQ(reloaded.productionQueueCount(), 2);
    EXPECT_EQ(reloaded.getQueue()[0].orderId, "ORD-002");
}

// T6-08: getCurrentJob — 큐에 항목 있을 때
TEST_F(Phase6Test, T6_08_GetCurrentJob) {
    SampleRepository    sr(samplePath);
    OrderRepository     or_(orderPath, queuePath);
    ProductionController ctrl(sr, or_);
    auto job = ctrl.getCurrentJob();
    ASSERT_TRUE(job.has_value());
    EXPECT_EQ(job->orderId, "ORD-001");
}
