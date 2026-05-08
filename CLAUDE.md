# CLAUDE.md — SampleOrderSystem

## 프로젝트 개요

반도체 시료 생산주문관리 시스템 (S-Semi)  
콘솔 기반 C++17 애플리케이션. MVC 아키텍처, JSON 파일 영속성, Google Test 기반 TDD.

- 기능 요구사항: `doc/requirements.md`
- 개발 계획: `doc/plan.md`

---

## 개발 프로세스 (TDD + Verify Harness)

각 Phase는 아래 순서를 반드시 따른다.

```
1. Red          테스트 먼저 작성 (실패 확인 필수)
      ↓
2. Green        테스트를 통과하는 최소 구현
      ↓
3. Verify       verify-all Agent 실행 → 통합 보고서 확인
      ↓
4. 검토 요청    보고서와 함께 사람에게 검토 요청
      ↓
5. 커밋 & 반복  검토 통과 후 커밋, 다음 Phase로
```

### Verify Harness (3단계)

검토 요청 전 반드시 `.claude/agents/verify-all.md` Agent를 실행한다.

| SubAgent | 역할 |
|----------|------|
| `verify-docs` | 문서 정합성 검증 (CLAUDE.md / requirements / design 간 충돌 탐지) |
| `verify-test` | 빌드 및 전체 테스트 실행, 결과 보고 |
| `verify-compliance` | 현재 Phase 완료 기준 체크리스트 준수 여부 확인 |
| `verify-all` | 위 3개 총괄 실행 → 통합 보고서 생성 |

verify-test와 verify-compliance는 병렬 실행 가능.  
**전체 PASS인 경우에만 사람에게 검토를 요청한다.**

### 검토 요청 시 제시 항목

- verify-all 통합 보고서
- 구현 파일 목록 및 파일별 변경 라인 수
- Phase 설계 문서 완료 기준 체크리스트

---

## PLAN 문서 관리

- 작업별 PLAN 문서는 TDD 단계에서 커밋 대상에 포함한다.
- **PR 생성 시에는 PLAN 문서를 제거하고 PR을 올린다.**
- PLAN 문서 위치: `plans/` 디렉토리 (`.gitignore`에 별도 관리 가능)

---

## PR 생성 형식

PR 생성 시 아래 내용을 PR description에 포함한다.

```
## 작업 내용
- 요청 받은 작업 및 요구사항 정리

## 해석 및 구현 방향
- 요구사항에서 파악한 의도

## 변경 내용 요약
- 주요 변경 사항

## 핵심 코드
- 중요한 로직 요약

## 요청사항 충족 체크리스트
- [ ] 항목 1
- [ ] 항목 2

## 변경 파일 목록
| 파일 | 변경 라인 |
|------|---------|
| ...  | +N / -N |

## Verify 결과
- 문서 정합성: PASS/FAIL
- 테스트: N/N 통과
- 요구사항 준수: PASS/FAIL
```

---

## 빌드 & 테스트

```
# 1. NuGet 패키지 복원 (최초 1회 또는 packages/ 없을 때)
nuget.exe restore SampleOrderSystem.sln -PackagesDirectory packages

# 2. 빌드 (Visual Studio 2022에서 SampleOrderSystem.sln 열기 → 빌드)
# MSBuild 경로: C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe

# 3. 테스트 실행
bin\Debug\SampleOrderSystemTests.exe

# 4. 커버리지 측정 (결과: Coverage\index.html)
OpenCppCoverage.exe --sources src --export_type=html:Coverage -- bin\Debug\SampleOrderSystemTests.exe
```

---

## 아키텍처 규칙

아래 구조와 기술은 사전 PoC에서 검증된 방식을 그대로 적용한다.  
구현 시 각 PoC repo의 코드를 참고한다.

| 레이어 | 책임 |
|--------|------|
| Model | 도메인 데이터 구조 및 비즈니스 규칙 |
| Repository | JSON 파일 읽기/쓰기 (영속성) |
| Controller | 유스케이스 로직, Model/Repository 조합 |
| View | 콘솔 입출력만 담당, 비즈니스 로직 없음 |

- MVC 구조: https://github.com/secrbs/ConsoleMVC-AnJunHo-14024910 참고
- CRUD: https://github.com/secrbs/DataPersistence-AnJunHo-14024910 활용
- 모니터링 기능 구현 시: https://github.com/secrbs/DataMonitor-AnJunHo-14024910 참고
- 테스트 데이터 생성 시: https://github.com/secrbs/DummyDataGenerator-AnJunHo-14024910 활용

규칙:
- Controller는 View를 직접 참조하지 않는다.
- View는 비즈니스 로직을 포함하지 않는다.
- Repository는 파일 I/O만 담당한다.

## 코드 규칙

- 언어: C++17
- 빌드: Visual Studio 2022 (v143, x64)
- 테스트: Google Test (NuGet)
- JSON: nlohmann/json (`include/nlohmann/json.hpp`)
- 데이터: `data/samples.json`, `data/orders.json`, `data/production_queue.json`
- 주석: 비자명한 이유가 있을 때만 작성
- `using namespace std;` 전역 사용 금지

---

## Phase 현황

| Phase | 명칭 | 상태 |
|-------|------|------|
| Phase 1 | 메인 UI (뼈대) | 완료 |
| Phase 2 | 시료 관리 | 완료 |
| Phase 3 | 주문 접수 | 완료 |
| Phase 4 | 주문 승인/거절 | 완료 |
| Phase 5 | 생산라인 | 완료 |
| Phase 6 | 모니터링 | 완료 |
| Phase 7 | 출고 처리 | 완료 |
| Phase 8 | 메인 UI 통합 | 미시작 |
