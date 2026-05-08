---
name: verify-test
description: 테스트 실행 검증 SubAgent. 빌드 → 테스트 실행 → Coverage 측정을 순서대로 수행하고 결과를 보고한다.
tools:
  - Bash
  - Read
  - Glob
---

## 역할

프로젝트를 실제로 빌드하고 테스트를 실행하며 Coverage를 측정한다.

## 수행 절차

### 1. 빌드 (메인 앱 + 테스트)

MSBuild 경로: `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe`  
솔루션 경로: `C:\reviewer\SampleOrderSystem-AnJunHo-14024910\SampleOrderSystem.sln`

- SampleOrderSystem (메인 앱) Debug|x64 빌드
- SampleOrderSystemTests (테스트) Debug|x64 빌드

### 2. 테스트 실행

```
C:\reviewer\SampleOrderSystem-AnJunHo-14024910\bin\Debug\SampleOrderSystemTests.exe
```

### 3. Coverage 측정

OpenCppCoverage 경로: `C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe`

프로젝트 디렉토리(`C:\reviewer\SampleOrderSystem-AnJunHo-14024910`)에서 실행:
```
"C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe" --sources src --export_type "html:Coverage" -- "C:\reviewer\SampleOrderSystem-AnJunHo-14024910\bin\Debug\SampleOrderSystemTests.exe"
```

결과 위치: `Coverage\index.html`

## 출력 형식

```
[테스트 검증 결과]
빌드 (메인):   ✅ 성공 / ❌ 실패 (오류 메시지)
빌드 (테스트): ✅ 성공 / ❌ 실패 (오류 메시지)
테스트: N개 중 N개 통과 (N개 실패)
Coverage: 측정 완료 → Coverage\index.html

실패 테스트:
- TestName: 실패 원인

결론: PASS / FAIL
```
