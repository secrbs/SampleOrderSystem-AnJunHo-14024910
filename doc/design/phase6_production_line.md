# Phase 6: 생산라인

## 목표

FIFO 생산 큐를 구현하고, 실생산량 계산 및 시간 경과에 따른 자동 생산 완료(PRODUCING → CONFIRMED)를 구현한다.

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
    int shortage;           // 부족분
    int actualProduction;   // ceil(shortage / (yield * 0.9))
    double totalTime;       // avgProductionTime * actualProduction (분)
    std::string enqueuedAt; // 큐 등록 시각 (ISO 8601)
};
```

## 구현 범위

### 생산 자동 완료
- 메인 루프 실행 시마다 현재 작업의 경과 시간 확인
- 경과 시간 ≥ totalTime(분) 이면 자동 완료 처리
  - 재고 += actualProduction
  - 주문 상태: PRODUCING → CONFIRMED
  - 큐에서 제거 → 다음 작업 자동 시작

### 생산 현황 표기 (조회 전용 — [5] 생산라인 조회)
- 현재 생산 중인 작업 정보 표시
  - 주문번호, 시료, 부족분, 실생산량, 총생산시간, 경과시간, 잔여시간
- 대기 중인 생산 큐 목록 출력 (FIFO 순서)
- **수동 완료 처리 없음** — 시간 경과 시 자동 처리

## 테스트 시나리오

| ID | 내용 | 예상 결과 |
|----|------|-----------|
| T6-01 | 실생산량 계산 (부족분 170, 수율 0.92) | 206 |
| T6-02 | 큐 enqueue 3건 후 dequeue 순서 | FIFO 순서 보장 |
| T6-03 | 빈 큐 상태 조회 | 빈 큐 반환 |
| T6-04 | 생산 완료 처리 | 재고 증가, 상태 CONFIRMED |
| T6-05 | 생산 완료 후 다음 작업 자동 전환 | 큐 다음 항목이 current로 이동 |
| T6-06 | 총생산시간 계산 | avgTime × actualProduction |
| T6-07 | 큐 영속성: 재시작 후 큐 복원 | 동일 큐 상태 유지 |
| T6-08 | getCurrentJob — 큐에 항목 있을 때 | front 반환 |

## 완료 기준

- [ ] ProductionLine 모델, FIFO 큐 구현
- [ ] 실생산량 / 총생산시간 계산 로직
- [ ] 생산 완료 시 재고 반영 및 상태 전환
- [ ] 큐 JSON 영속성
- [ ] 시간 경과 기반 자동 완료 처리 (미구현)
- [ ] 생산 현황 View 수정 — 조회 전용 + 잔여시간 표시 (미구현)
- [ ] 모든 테스트 Green
