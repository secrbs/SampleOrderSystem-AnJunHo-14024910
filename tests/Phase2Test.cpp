#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "controller/SampleController.h"
#include "repository/SampleRepository.h"

static void writeJson(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

class Phase2Test : public ::testing::Test {
protected:
    void SetUp() override {
        writeJson(path, "[]");
    }
    void TearDown() override {
        std::remove(path.c_str());
    }
    const std::string path = "test_samples_p2.json";
};

// T2-01: 유효한 시료 등록
TEST_F(Phase2Test, T2_01_RegisterValid) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    Sample s{"S-001", "실리콘 웨이퍼", 0.5, 0.92, 0};
    EXPECT_TRUE(ctrl.registerSample(s));
    EXPECT_EQ(repo.count(), 1);
}

// T2-02: 중복 ID 등록 불가
TEST_F(Phase2Test, T2_02_RegisterDuplicateId) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    Sample s{"S-001", "웨이퍼A", 0.5, 0.9, 0};
    ctrl.registerSample(s);
    EXPECT_FALSE(ctrl.registerSample(s));
    EXPECT_EQ(repo.count(), 1);
}

// T2-03: 수율 0 이하 등록 불가
TEST_F(Phase2Test, T2_03_RegisterYieldZero) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    Sample s{"S-001", "웨이퍼A", 0.5, 0.0, 0};
    EXPECT_FALSE(ctrl.registerSample(s));
    EXPECT_EQ(repo.count(), 0);
}

// T2-04: 수율 1.0 초과 등록 불가
TEST_F(Phase2Test, T2_04_RegisterYieldOver1) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    Sample s{"S-001", "웨이퍼A", 0.5, 1.1, 0};
    EXPECT_FALSE(ctrl.registerSample(s));
    EXPECT_EQ(repo.count(), 0);
}

// T2-05: 시료 없는 상태 목록 조회
TEST_F(Phase2Test, T2_05_ListEmpty) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    EXPECT_TRUE(ctrl.getAll().empty());
}

// T2-06: 시료 등록 후 목록 조회
TEST_F(Phase2Test, T2_06_ListAfterRegister) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    ctrl.registerSample({"S-001", "웨이퍼A", 0.5, 0.9, 0});
    ctrl.registerSample({"S-002", "웨이퍼B", 0.3, 0.8, 0});
    auto list = ctrl.getAll();
    EXPECT_EQ(list.size(), 2u);
}

// T2-07: 이름 부분 일치 검색 (존재)
TEST_F(Phase2Test, T2_07_SearchByNameFound) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    ctrl.registerSample({"S-001", "실리콘 웨이퍼", 0.5, 0.9, 0});
    ctrl.registerSample({"S-002", "GaN 에피택셜", 0.3, 0.8, 0});
    auto result = ctrl.searchByName("웨이퍼");
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].id, "S-001");
}

// T2-08: 이름 검색 결과 없음
TEST_F(Phase2Test, T2_08_SearchByNameNotFound) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    ctrl.registerSample({"S-001", "실리콘 웨이퍼", 0.5, 0.9, 0});
    EXPECT_TRUE(ctrl.searchByName("없는시료").empty());
}

// T2-09: ID로 단건 조회
TEST_F(Phase2Test, T2_09_FindById) {
    SampleRepository repo(path);
    SampleController ctrl(repo);
    ctrl.registerSample({"S-001", "웨이퍼A", 0.5, 0.9, 0});
    auto s = ctrl.findById("S-001");
    ASSERT_TRUE(s.has_value());
    EXPECT_EQ(s->name, "웨이퍼A");
}

// T2-10: JSON 저장 후 재로드
TEST_F(Phase2Test, T2_10_PersistAndReload) {
    {
        SampleRepository repo(path);
        SampleController ctrl(repo);
        ctrl.registerSample({"S-001", "웨이퍼A", 0.5, 0.9, 100});
    }
    SampleRepository reloaded(path);
    EXPECT_EQ(reloaded.count(), 1);
    EXPECT_EQ(reloaded.findAll()[0].name, "웨이퍼A");
}
