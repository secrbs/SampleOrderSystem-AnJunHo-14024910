# SampleOrderSystem — 개발 계획

## 아키텍처

MVC 패턴 기반 콘솔 C++17 애플리케이션.

```
src/
├── model/          - 도메인 데이터 구조
├── repository/     - JSON 파일 영속성 (CRUD)
├── controller/     - 유스케이스 로직
├── view/           - 콘솔 입출력
└── main.cpp
tests/              - Google Test
data/               - samples.json / orders.json / production_queue.json
include/nlohmann/   - json.hpp
```

## Phase 목록

| Phase | 명칭 | 설계 문서 | 상태 |
|-------|------|-----------|------|
| Phase 1 | 시료 관리 | `doc/design/phase1_sample_management.md` | 미시작 |
| Phase 2 | 주문 접수 | `doc/design/phase2_order_reservation.md` | 미시작 |
| Phase 3 | 주문 승인/거절 | `doc/design/phase3_order_approval.md` | 미시작 |
| Phase 4 | 생산라인 | `doc/design/phase4_production_line.md` | 미시작 |
| Phase 5 | 모니터링 | `doc/design/phase5_monitoring.md` | 미시작 |
| Phase 6 | 출고 처리 | `doc/design/phase6_shipment.md` | 미시작 |
