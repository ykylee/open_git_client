# Open Git Client

Windows를 주요 타겟으로 하는 고성능 Git 클라이언트 프로젝트다. Linux에서도 동일 소스 트리를 빌드할 수 있도록 유지하며, Git 작업 속도와 UI 반응성을 최우선 목표로 둔다.

## 현재 상태
- 현재 개발 버전: `v0.0.2`
- 현재 단계: 초기 구현 착수 및 앱 골격 구성
- 구현 상태: `Qt Widgets` 기반 메인 윈도우, 세션/유스케이스 골격, 임시 Git 백엔드, 기본 단위 테스트와 빌드 검증까지 완료

## 프로젝트 방향
- 제품 유형: 데스크톱 Git 클라이언트
- 참고 제품: `GitKraken`
- 주요 타겟 플랫폼: Windows
- 빌드 호환 목표: Linux 빌드 가능 유지
- 핵심 우선순위: Git 작업 처리 속도, 비차단 UI, 대용량 저장소 대응

## 예정 기술 스택
- 언어: `C++20`
- UI: `Qt 6` (`Qt Widgets` 우선)
- 빌드: `CMake + Ninja`
- Git 연동: `libgit2` 중심, 필요 시 네이티브 `git` CLI 병행
- Windows 개발 환경: `MSVC 2022`
- Linux 빌드 환경: `GCC` 또는 `Clang`

## 주요 문서
- [CRS](docs/requirements/CRS.md): 고객 요구사항 명세
- [SRS](docs/requirements/SRS.md): 시스템 요구사항 명세
- [Initial Design Specification](docs/design/initial-design-specification.md): Release 1 초기 설계 명세
- [Initial Technical Architecture](docs/design/initial-technical-architecture.md): 초기 기술 아키텍처 초안
- [GitKraken Feature Analysis](docs/design/gitkraken-feature-analysis.md): 참고 제품 기능 분석
- [Thread Operating Guidelines](docs/thread-operating-guidelines.md): 현재 문서화 스레드 운영 규칙

## 초기 구현 범위
- 저장소 열기, 복제, 초기화
- 다중 저장소 탐색
- 커밋 그래프 및 참조 시각화
- 작업 트리 보기와 선택적 스테이징
- 커밋 작성, amend, revert, reset
- 브랜치 관리, merge, rebase, 충돌 해결
- remote 관리와 `fetch`/`pull`/`push`
- 검색, diff, file history, blame

## 현재 구현된 내용
- `CMake + Ninja + Qt 6` 기반 프로젝트 빌드 진입점 구성
- `RepositorySession`, `TaskCoordinator`, `RepositoryUseCases` 중심의 Phase 1 런타임 골격 추가
- 메인 윈도우, 사이드바, 커밋 그래프, 인스펙터, 작업 트리 패널의 초기 UI 스켈레톤 구현
- 실제 Git 연동 전 단계로 사용할 in-memory Git backend 추가
- `QtTest` 기반 단위 테스트 2종 추가 및 Linux 환경에서 configure/build/ctest 검증 완료

## 빌드 의존성
- 필수 도구: `build-essential`, `cmake`, `ninja-build`, `pkg-config`
- 필수 라이브러리: `qt6-base-dev`, `libgit2-dev`
- 최소 지원 Qt 버전: `6.4.2`
- 현재 Linux 검증 기준 버전: `Qt 6.9.2`, `libgit2 1.9.0`

Ubuntu 기준 설치 예시:

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  ninja-build \
  pkg-config \
  qt6-base-dev \
  libgit2-dev
```

## Windows 빌드 환경
- 권장 운영체제: `Windows 10 (1809+)` 또는 `Windows 11`
- 권장 컴파일러: `MinGW-w64`
- 권장 Qt: `Qt 6.4.2` 이상, 반드시 `MinGW 64-bit` 키트로 설치
- 필수 도구: `CMake`, `Ninja`, `Git`
- 배포 도구: `windeployqt`

권장 설치 순서:

1. `MinGW-w64` 툴체인이 포함된 개발 환경을 준비한다.
2. `Qt`를 설치할 때 `MinGW 64-bit`용 `Qt 6.4.2+`와 `Tools > Ninja`, `CMake` 사용 가능 환경을 준비한다.
3. Qt가 제공하는 `MinGW 64-bit` 환경 프롬프트 또는 `mingw64` 컴파일러가 PATH에 잡힌 셸에서 빌드한다.
4. `windeployqt`가 PATH에 없으면 Qt 설치 경로의 `bin` 디렉터리를 환경에 추가한다.

Windows 빌드 예시:

```bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows 포터블 패키지 생성:

```bat
cmake --build build --target package_open_git_client_portable
```

- 일반 빌드 결과 앱 실행 파일은 `build/bin/OpenGitClient.exe`에 생성된다.
- 포터블 패키지 폴더는 `build/package/OpenGitClient/`에 생성된다.
- ZIP 파일은 `build/dist/OpenGitClient-<version>-Windows.zip` 형태로 생성된다.

## 빌드와 테스트
```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Linux에서 빌드 전 산출물 정리:

```bash
./scripts/clean_linux_build.sh
```

- 기본 앱 실행 파일 출력 경로: `build/bin/OpenGitClient` (`Windows`에서는 `OpenGitClient.exe`)
- 테스트 실행 파일 출력 경로: `build/tests/`

배포용 런타임 번들 정리:

```bash
cmake --build build --target bundle_open_git_client
```

- 일반 빌드 결과 앱 실행 파일은 `build/bin/OpenGitClient`에 생성된다.
- 배포 번들 타깃은 실행 파일과 런타임 의존성을 `build/package/OpenGitClient/bin` 기준으로 정리한다.
- Windows에서는 `windeployqt` 기반으로 포터블 폴더를 구성한다.
- Linux에서는 Qt deploy script 기반으로 포터블 폴더를 구성한다.

포터블 배포 폴더를 ZIP까지 한 번에 만들기:

```bash
cmake --build build --target package_open_git_client_portable
```

- 이 타깃은 먼저 `build/package/OpenGitClient/` 폴더를 구성한다.
- 그 다음 `build/dist/OpenGitClient-<version>-<platform>.zip` 파일을 생성한다.
- Windows에서는 이 `OpenGitClient` 폴더만 복사해도 바로 실행 가능한 포터블 배포 형태를 목표로 한다.

## 버전 정책
- 현재 기준 개발 버전은 `v0.0.2`이다.
- 버전 번호는 사용자 지시가 있을 때만 올린다.
- 모든 문서의 변경 이력은 프로젝트 개발 버전을 기준으로 기록한다.
- 버전이 올라갈 때마다 아래 변경 이력을 함께 갱신한다.

## 변경 이력

### `v0.0.2` (개발 중)
- 2026-03-11: `Qt Widgets` 기반 초기 애플리케이션 골격, `RepositorySession`/`TaskCoordinator`/유스케이스 구조, in-memory Git backend, 단위 테스트와 Linux 빌드 검증을 추가하고 README에 현재 구현 상태와 빌드 방법을 반영했다.

### `v0.0.1`
- 2026-03-11: 프로젝트 개요와 버전 정책을 정리하고, `CRS`, `SRS`, 초기 설계 명세, 초기 기술 아키텍처, `GitKraken` 기능 분석, 스레드 운영 문서를 작성했으며 모든 문서의 변경 이력을 개발 버전 기준으로 통일했다.
