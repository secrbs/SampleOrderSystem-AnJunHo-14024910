# CLAUDE.md — SampleOrderSystem

## 프로젝트 개요

반도체 시료 생산주문관리 시스템 (S-Semi)  
콘솔 기반 C++17 애플리케이션. MVC 아키텍처, JSON 파일 영속성, Google Test 기반 TDD.

- 기능 요구사항: `doc/requirements.md`
- 개발 계획: `doc/plan.md`
- Phase별 설계: `doc/design/phase*.md`

---

## 개발 프로세스 (TDD + Phase 기반)

각 Phase는 아래 순서를 반드시 따른다.

1. **Red** — 테스트 먼저 작성 (구현 없으므로 반드시 실패)
2. **Green** — 테스트를 통과하는 최소 구현
3. **검토 요청** — 사용자 검토 없이 커밋 및 다음 Phase 진입 금지
4. **커밋 & 반복** — 검토 통과 후 커밋, 다음 Phase로

검토 요청 시 함께 제시:
- 구현 파일 목록
- 테스트 실행 결과 (전체 Green)
- 설계 문서 완료 기준 체크리스트

---

## 빌드 & 테스트

```
# Visual Studio 2022에서 SampleOrderSystem.sln 열기 → 빌드
# 테스트 실행
bin\Debug\SampleOrderSystemTests.exe
```

---

## 아키텍처 규칙

아래 구조와 기술은 사전 PoC에서 검증된 방식을 그대로 적용한다.

| 레이어 | 책임 | 검증 PoC |
|--------|------|----------|
| Model | 도메인 데이터 구조 및 비즈니스 규칙 | - |
| Repository | JSON 파일 읽기/쓰기 (영속성) | PoC-2 DataPersistence |
| Controller | 유스케이스 로직, Model/Repository 조합 | PoC-1 ConsoleMVC |
| View | 콘솔 입출력만 담당, 비즈니스 로직 없음 | PoC-1 ConsoleMVC |

- MVC 레이어 분리 방식: PoC-1(ConsoleMVC)에서 검증
- JSON 영속성 (nlohmann/json 기반 Repository): PoC-2(DataPersistence)에서 검증
- 모니터링 화면 구성 (주문 현황 + 재고 상태): PoC-3(DataMonitor)에서 검증
- 테스트용 더미 데이터 생성: PoC-4(DummyDataGenerator) 활용

규칙:
- Controller는 View를 직접 참조하지 않는다.
- View는 비즈니스 로직을 포함하지 않는다.
- Repository는 파일 I/O만 담당한다.

## 코드 규칙

- 언어: C++17
- 빌드: Visual Studio 2022 (v143, x64)
- 테스트: Google Test (NuGet)
- JSON: nlohmann/json (`include/nlohmann/json.hpp`) — PoC-2에서 검증
- 데이터: `data/samples.json`, `data/orders.json`, `data/production_queue.json`
- 주석: 비자명한 이유가 있을 때만 작성
- `using namespace std;` 전역 사용 금지

---

## Phase 현황

| Phase | 명칭 | 상태 |
|-------|------|------|
| Phase 1 | 시료 관리 | 미시작 |
| Phase 2 | 주문 접수 | 미시작 |
| Phase 3 | 주문 승인/거절 | 미시작 |
| Phase 4 | 생산라인 | 미시작 |
| Phase 5 | 모니터링 | 미시작 |
| Phase 6 | 출고 처리 | 미시작 |
