# SampleOrderSystem

반도체 시료 생산주문관리 시스템

## 개요

가상 반도체 회사 S-Semi의 시료(Sample) 생산주문관리 콘솔 애플리케이션.  
주문 접수 → 승인/거절 → 생산 → 출고까지 전 과정을 관리한다.

## 주요 기능

| 기능 | 설명 |
|------|------|
| 시료 관리 | 시료 등록 / 목록 조회 / 검색 |
| 시료 주문 | 고객 주문 접수 (RESERVED) |
| 주문 승인/거절 | 재고 기반 상태 자동 전환 |
| 모니터링 | 상태별 주문 수, 시료별 재고 현황 |
| 생산라인 | FIFO 큐, 실생산량 계산 |
| 출고 처리 | CONFIRMED → RELEASE |

## 기술 스택

| 항목 | 내용 |
|------|------|
| 언어 | C++17 |
| 빌드 | Visual Studio 2022 |
| 테스트 | Google Test |
| 데이터 | JSON 파일 (nlohmann/json) |

## 빌드

`SampleOrderSystem.sln` 을 Visual Studio 2022에서 열어 빌드.

## 문서

- `doc/requirements.md` — 기능 요구사항
- `doc/design/phase*.md` — 각 Phase 설계 문서
