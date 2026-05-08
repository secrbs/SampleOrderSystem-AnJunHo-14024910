# Phase 7: 출고 처리

## 목표

CONFIRMED 상태 주문에 대해 출고를 실행하고 상태를 RELEASE로 전환한다.

## 구현 범위

- CONFIRMED 상태 주문 목록 표시
- 선택한 주문에 대해 출고 실행
  - 상태: CONFIRMED → RELEASE
  - 출고 일시 기록
- 출고 시 재고 차감

## 테스트 시나리오

| ID | 내용 | 예상 결과 |
|----|------|-----------|
| T7-01 | CONFIRMED 주문 출고 실행 | 상태 RELEASE로 전환, 재고 차감 |
| T7-02 | CONFIRMED 아닌 주문 출고 시도 | false 반환, 상태 변경 없음 |
| T7-03 | 존재하지 않는 주문 출고 시도 | false 반환 |
| T7-04 | CONFIRMED 목록 조회 | CONFIRMED 상태 주문만 반환 |
| T7-05 | JSON 저장 후 재로드 | RELEASE 상태 유지 |

## 완료 기준

- [ ] 출고 처리 로직 (CONFIRMED → RELEASE)
- [ ] ShipmentView 콘솔 UI 구현
- [ ] 모든 테스트 Green
