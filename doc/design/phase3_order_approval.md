# Phase 3: 주문 승인/거절

## 목표

RESERVED 주문에 대해 승인 또는 거절을 처리하고, 재고 상황에 따라 상태를 자동 전환한다.

## 상태 전환 규칙

```
RESERVED ──[approve, 재고 충분]──→ CONFIRMED   (재고 차감)
RESERVED ──[approve, 재고 부족]──→ PRODUCING   (생산라인 큐 등록)
RESERVED ──[reject]──────────────→ REJECTED
```

## 구현 범위

### 접수 주문 목록
- RESERVED 상태 주문 목록 표시

### 주문 승인
- 재고 확인
  - **재고 충분** (재고 ≥ 주문 수량): 재고 차감 → CONFIRMED
  - **재고 부족** (재고 < 주문 수량): 생산라인 큐 등록 → PRODUCING

### 주문 거절
- 즉시 REJECTED 전환

## 테스트 시나리오

| ID | 내용 | 예상 결과 |
|----|------|-----------|
| T3-01 | 재고 충분할 때 승인 | CONFIRMED, 재고 차감 |
| T3-02 | 재고 부족할 때 승인 | PRODUCING, 생산라인 큐 등록 |
| T3-03 | 재고 정확히 일치할 때 승인 | CONFIRMED (충분 조건) |
| T3-04 | 주문 거절 | REJECTED |
| T3-05 | CONFIRMED 주문 재승인 시도 | 오류, 상태 변경 없음 |
| T3-06 | RESERVED 목록 조회 | RESERVED 상태 주문만 반환 |

## 완료 기준

- [ ] 승인 시 재고 차감 및 상태 전환 로직
- [ ] 재고 부족 시 생산라인 큐 enqueue 연동
- [ ] 거절 처리 로직
- [ ] 모든 테스트 Green
