# E2E 테스트 환경

## 사전 준비

### 1. 빌드

**SampleOrderSystem (메인 앱)**
```
SampleOrderSystem.sln → Visual Studio 2022에서 빌드 (Debug|x64)
출력: bin\Debug\SampleOrderSystem.exe
```

**DummyDataGenerator (테스트 데이터 생성)**
```
C:\reviewer\DummyDataGenerator-AnJunHo-14024910\DummyDataGenerator.sln
→ Visual Studio 2022에서 빌드 (Debug|x64)
출력: x64\Debug\DummyDataGenerator.exe
```

### 2. 테스트 데이터 생성

DummyDataGenerator로 샘플 데이터를 생성한다.

```
DummyDataGenerator.exe "C:\reviewer\SampleOrderSystem-AnJunHo-14024910\data" --reset
```

생성 파일:
| 파일 | 내용 |
|------|------|
| `data/samples.json` | 시료 10종 (S-001~S-010, 수율/재고/생산시간 다양) |
| `data/orders.json` | 주문 10건 (RESERVED/CONFIRMED/PRODUCING/RELEASE/REJECTED 혼합) |
| `data/production_queue.json` | 생산 큐 3건 |

> `data/*.json`은 `.gitignore` 대상이므로 클론 후 매번 생성 필요

---

## 실행

```
bin\Debug\SampleOrderSystem.exe
```

---

## E2E 시나리오

### 시나리오 1: 시료 등록 → 주문 → 승인(재고 충분) → 출고
1. `[1] 시료 관리` → `[1] 시료 등록` → 새 시료 입력
2. `[2] 시료 주문` → 시료 ID, 고객명, 수량 입력
3. `[3] 주문 승인/거절` → RESERVED 주문 승인 → CONFIRMED 확인
4. `[6] 출고 처리` → CONFIRMED 주문 출고 → RELEASE 확인

### 시나리오 2: 주문 → 승인(재고 부족) → 생산 완료 → 출고
1. `[2] 시료 주문` → 재고보다 많은 수량 주문
2. `[3] 주문 승인/거절` → 승인 → PRODUCING 확인, 생산라인 등록 확인
3. `[5] 생산라인 조회` → `[C] 생산 완료 처리` → CONFIRMED 확인
4. `[6] 출고 처리` → 출고 → RELEASE 확인

### 시나리오 3: 모니터링 확인
1. `[1] 시료 관리` → `[2] 시료 목록` → 재고 현황 확인
2. `[4] 모니터링` → 상태별 주문 수 및 재고 여유/부족/고갈 확인

### 시나리오 4: 주문 거절
1. `[3] 주문 승인/거절` → RESERVED 주문 거절(`N`) → REJECTED 확인
2. `[4] 모니터링` → 거절된 주문은 집계에서 제외됨 확인

---

## 데이터 초기화

테스트 데이터를 다시 초기화하려면 DummyDataGenerator를 `--reset` 옵션으로 다시 실행한다.
