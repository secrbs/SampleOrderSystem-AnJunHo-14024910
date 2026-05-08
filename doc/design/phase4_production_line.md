# Phase 4: 생산라인

## 목표

FIFO 생산 큐를 구현하고, 실생산량 계산 및 생산 완료 처리(PRODUCING → CONFIRMED)를 구현한다.

## 생산량 계산 공식

```
실생산량  = ceil(부족분 / (수율 × 0.9))
총생산시간 = 평균생산시간(min/ea) × 실생산량
```

예시) 부족분 170, 수율 0.92, 평균생산시간 0.8 min/ea
```
실생산량  = ceil(170 / (0.92 × 0.9)) = ceil(205.31) = 206
총생산시간 = 0.8 × 206 = 164.8 min
```

## 도메인 모델

```cpp
struct ProductionJob {
    std::string orderId;
    std::string sampleId;
    int orderQuantity;
    int stockAtApproval;
    int shortage;           // orderQuantity - stockAtApproval
    int actualProduction;   // ceil(shortage / (yield * 0.9))
    double totalTime;       // avgProductionTime * actualProduction
    std::string enqueuedAt;
};
```

## 구현 범위

- FIFO 생산 큐 (enqueue / dequeue)
- 생산 현황: 현재 처리 중인 작업 정보 표시
- 대기 목록: 큐에 대기 중인 작업 목록 출력
- 생산 완료 처리: 재고 증가 + PRODUCING → CONFIRMED

## 테스트 시나리오

| ID | 내용 | 예상 결과 |
|----|------|-----------|
| T4-01 | 실생산량 계산 (부족분 170, 수율 0.92) | 206 |
| T4-02 | 큐 enqueue 3건 후 dequeue 순서 | FIFO 순서 보장 |
| T4-03 | 빈 큐 현황 조회 | "대기 없음" 출력 |
| T4-04 | 생산 완료 처리 | 재고 증가, 상태 CONFIRMED |
| T4-05 | 생산 완료 후 다음 작업 자동 전환 | 큐 다음 항목이 current로 이동 |
| T4-06 | 총생산시간 계산 | avgTime × actualProduction |
| T4-07 | 큐 영속성: 재시작 후 큐 복원 | 동일 큐 상태 유지 |

## 완료 기준

- [ ] ProductionLine 모델, FIFO 큐 구현
- [ ] 실생산량 / 총생산시간 계산 로직
- [ ] 생산 완료 시 재고 반영 및 상태 전환
- [ ] 큐 JSON 영속성
- [ ] 모든 테스트 Green
