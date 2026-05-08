# Phase 8: 메인 UI 통합

## 목표

Phase 2~7에서 구현한 Controller/View를 메인 메뉴에 실제로 연결하고,
누락된 ApprovalView와 ProductionView를 구현한다.

## 현재 상태

| 메뉴 | Controller | View | 상태 |
|------|-----------|------|------|
| [1] 시료 관리 | SampleController | SampleView | stub |
| [2] 시료 주문 | OrderController | OrderView | stub |
| [3] 주문 승인/거절 | ApprovalController | **없음** | stub |
| [4] 모니터링 | MonitoringController | MonitorView | stub |
| [5] 생산라인 조회 | ProductionController | **없음** | stub |
| [6] 출고 처리 | ShipmentController | ShipmentView | stub |

## 구현 범위

### ApprovalView (신규)
- RESERVED 주문 목록 출력
- 승인/거절 대상 주문번호 선택 입력
- 승인 결과(CONFIRMED / PRODUCING) 출력
- 거절 결과(REJECTED) 출력

### ProductionView (신규)
- 현재 생산 중인 작업 정보 표시 (주문번호, 시료, 실생산량, 총생산시간)
- 대기 중인 생산 큐 목록 출력 (FIFO 순서)
- 생산 완료 처리 입력 ([C] 완료 처리)

### main.cpp 통합
- 각 메뉴 선택 시 해당 Controller + View 호출
- SampleRepository, OrderRepository를 모든 Controller에 주입
- 기능 수행 후 메인 메뉴로 복귀

## 테스트 시나리오

| ID | 내용 | 예상 결과 |
|----|------|-----------|
| T8-01 | 메뉴 [1] → 시료 관리 서브메뉴 진입 | SampleView 출력 |
| T8-02 | 메뉴 [2] → 시료 주문 서브메뉴 진입 | OrderView 출력 |
| T8-03 | 메뉴 [3] → RESERVED 목록 표시 | ApprovalView 출력 |
| T8-04 | 메뉴 [3] → 승인 처리 | 재고 충분 시 CONFIRMED, 부족 시 PRODUCING |
| T8-05 | 메뉴 [3] → 거절 처리 | REJECTED |
| T8-06 | 메뉴 [4] → 모니터링 화면 출력 | MonitorView 출력 |
| T8-07 | 메뉴 [5] → 생산라인 현황 출력 | ProductionView 출력 |
| T8-08 | 메뉴 [5] → 생산 완료 처리 | 재고 증가, PRODUCING→CONFIRMED |
| T8-09 | 메뉴 [6] → CONFIRMED 주문 목록 출력 | ShipmentView 출력 |
| T8-10 | 메뉴 [6] → 출고 처리 | CONFIRMED→RELEASE |

## 완료 기준

- [ ] ApprovalView 구현
- [ ] ProductionView 구현
- [ ] main.cpp에서 모든 메뉴 실제 기능 연결
- [ ] stub 코드 제거
- [ ] 모든 테스트 Green
