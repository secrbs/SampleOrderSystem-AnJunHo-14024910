# Phase 3: 주문 접수

## 목표

고객의 시료 요청을 받아 주문(RESERVED)을 생성하는 기능을 구현한다.

## 도메인 모델

```cpp
enum class OrderStatus { RESERVED, REJECTED, PRODUCING, CONFIRMED, RELEASE };

struct Order {
    std::string orderId;      // 예: "ORD-20260508-0001"
    std::string sampleId;
    std::string customerName;
    int quantity;
    OrderStatus status;
    std::string createdAt;
    std::string updatedAt;
};
```

## 구현 범위

- 입력: 시료 ID, 고객명, 주문 수량
- 시료 ID 존재 여부 검증
- 주문 수량 > 0 검증
- 주문번호 자동 생성: `ORD-YYYYMMDD-NNNN` (당일 순번)
- 생성된 주문 상태: RESERVED
- 확인 후 [Y] 접수 / [N] 취소

## 테스트 시나리오

| ID | 내용 | 예상 결과 |
|----|------|-----------|
| T3-01 | 유효한 주문 접수 | RESERVED 상태 Order 생성 |
| T3-02 | 존재하지 않는 시료 ID | 실패, 오류 메시지 |
| T3-03 | 주문 수량 0 이하 | 유효성 검사 실패 |
| T3-04 | 같은 날 2건 주문 | 순번 -0001, -0002 각각 생성 |
| T3-05 | JSON 저장 후 재로드 | 동일 주문 복원 |

## 완료 기준

- [ ] Order 모델, OrderRepository 구현
- [ ] OrderController 주문 접수 로직 구현
- [ ] OrderView 콘솔 UI 구현
- [ ] 모든 테스트 Green
