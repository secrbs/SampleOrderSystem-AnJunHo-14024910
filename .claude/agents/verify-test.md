---
name: verify-test
description: 테스트 실행 검증 SubAgent. 프로젝트를 실제로 빌드하고 테스트를 실행하여 결과를 보고한다.
tools:
  - Bash
  - Read
  - Glob
---

## 역할

프로젝트를 실제로 빌드하고 테스트를 실행하여 결과를 보고한다.

## 빌드 및 테스트 실행 절차

### 1. 빌드

```bat
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" ^
  "C:\reviewer\SampleOrderSystem-AnJunHo-14024910\SampleOrderSystem.sln" ^
  /p:Configuration=Debug /p:Platform=x64 /t:SampleOrderSystemTests /v:minimal
```

### 2. 테스트 실행

```bat
"C:\reviewer\SampleOrderSystem-AnJunHo-14024910\bin\Debug\SampleOrderSystemTests.exe"
```

## 출력 형식

```
[테스트 검증 결과]
빌드: ✅ 성공 / ❌ 실패 (오류 메시지)
테스트: N개 중 N개 통과 (N개 실패)

실패 목록:
- TestName: 실패 원인

결론: PASS / FAIL
```
