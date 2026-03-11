# Initial Technical Architecture Draft

## 문서 정보
- 문서 버전: v0.1
- 작성일: 2026-03-11
- 기준 문서: `docs/requirements/CRS.md` v0.5, `docs/requirements/SRS.md` v0.5

## 목적
이 문서는 현재까지 확정된 요구사항을 바탕으로 Git 클라이언트의 초기 기술 아키텍처 방향을 정리한다. 구현 착수 전에 기술 선택과 모듈 경계를 고정하는 초안이다.

## 아키텍처 목표
- Windows 대상 데스크톱 Git 클라이언트를 제공한다.
- Linux 환경에서 동일 소스 트리를 빌드 가능하게 유지한다.
- Git 작업 처리 속도와 반응성을 최우선으로 설계한다.
- 호환성과 성능을 함께 확보하기 위해 `libgit2`와 네이티브 `git` 호출을 병행 가능한 구조로 만든다.

## 선택 기술 스택
- 언어: `C++20`
- UI 프레임워크: `Qt 6`
- 초기 UI 계층: `Qt Widgets`
- 빌드 시스템: `CMake + Ninja`
- Windows 주 개발 도구: `MSVC 2022`
- Linux 빌드 도구: `GCC` 또는 `Clang`
- Git 엔진: `libgit2`
- 보조 Git 실행 계층: 네이티브 `git` CLI

## 상위 구조
```text
+---------------------------+
| Qt Widgets UI             |
| - Repository Views        |
| - Commit/Branch Panels    |
| - Status/Log Views        |
+------------+--------------+
             |
             v
+---------------------------+
| Application Layer         |
| - Use Cases               |
| - Command Dispatcher      |
| - View Model / State      |
+------------+--------------+
             |
             v
+---------------------------+
| Git Service Layer         |
| - Repository Manager      |
| - Status Service          |
| - Log Service             |
| - Branch Service          |
| - Fetch/Push Service      |
+------+--------------+-----+
       |              |
       v              v
+--------------+   +----------------+
| libgit2      |   | Native git CLI |
| Fast local   |   | Compatibility  |
| operations   |   | complex ops    |
+--------------+   +----------------+
```

## 계층별 역할

### 1. UI 계층
- `Qt Widgets` 기반 데스크톱 화면을 제공한다.
- 저장소 상태, 브랜치, 커밋 로그, 작업 트리 변경 내용을 표시한다.
- 장시간 Git 작업은 비동기 요청으로 전달하고 진행 상태를 표시한다.

### 2. Application 계층
- UI 요청을 명시적 유스케이스로 변환한다.
- 요청 취소, 중복 실행 방지, 상태 동기화, 오류 전달을 담당한다.
- Git 작업 결과를 UI가 바로 사용할 수 있는 모델로 변환한다.

### 3. Git Service 계층
- 저장소별 작업 컨텍스트를 관리한다.
- 읽기 빈도가 높은 작업은 빠른 경로로 제공한다.
- 기능 호환성이 더 중요한 작업은 CLI 백엔드로 위임할 수 있다.

### 4. Git Backend 계층
- `libgit2` backend:
  로컬 저장소 상태 조회, 브랜치/커밋/인덱스 읽기 등 고빈도 작업을 처리한다.
- Native `git` backend:
  `fetch`, `push`, `rebase`, 일부 인증/credential helper 연동, 향후 `submodule`/`LFS` 확장 같은 작업을 맡는다.

## 성능 우선 설계 원칙
- UI 스레드에서는 Git 저장소 스캔과 대량 파싱을 직접 수행하지 않는다.
- 저장소 상태 조회는 캐시 가능한 단위로 분리한다.
- 대용량 로그/파일 목록은 전체 로딩 대신 점진적 로딩을 우선한다.
- 동일 저장소에 대한 중복 스캔 요청은 병합하거나 취소 가능해야 한다.
- CLI 호출은 필요한 경우에만 사용하고, 가능한 로컬 읽기 작업은 `libgit2`로 우선 처리한다.

## 동시성 전략 초안
- UI 메인 스레드와 Git 작업 스레드를 분리한다.
- `QThreadPool` 또는 동등한 작업 실행 계층으로 백그라운드 작업을 처리한다.
- 저장소 단위의 직렬화가 필요한 작업과 병렬 가능한 읽기 작업을 구분한다.
- 취소 가능한 작업 핸들을 두어 브랜치 전환, 로그 새로고침, 상태 재조회 요청을 제어한다.

## 초기 모듈 분해 초안
- `app`: 애플리케이션 시작점, DI 조립, 전역 설정
- `ui`: 메인 윈도우, 패널, 다이얼로그, 뷰
- `application`: 유스케이스, 상태 관리, 명령 디스패치
- `domain`: 저장소, 브랜치, 커밋, 작업 트리 모델
- `git-core`: Git 서비스 인터페이스
- `git-libgit2`: `libgit2` 구현체
- `git-cli`: 네이티브 `git` 실행 구현체
- `platform`: 프로세스 실행, 파일시스템, OS 추상화

## 빌드 및 배포 방향
- 루트 빌드는 `CMake` 단일 진입점으로 구성한다.
- 기본 로컬 빌드는 `Ninja`를 사용한다.
- Windows CI는 `MSVC 2022`, Linux CI는 `GCC` 또는 `Clang` 매트릭스로 운영한다.
- 플랫폼 종속 코드는 `platform` 계층에 격리한다.

## 리스크와 대응
- `libgit2`와 네이티브 `git` 간 결과 차이:
  기능별 백엔드 책임 범위를 명확히 분리한다.
- 대용량 저장소에서 UI 지연 발생:
  점진적 로딩과 작업 취소, 캐시 계층을 우선 설계한다.
- Windows 대상 앱인데 Linux 빌드가 깨질 가능성:
  초기부터 CI에 Linux 빌드를 포함한다.

## 다음 설계 상세화 항목
- 핵심 화면 구조와 패널 배치
- 저장소 상태 캐시 전략
- Git 작업 큐와 취소 모델
- 인증, 자격 증명, 원격 저장소 처리 방식
- 대용량 저장소 성능 측정 기준

## 변경 이력
- 2026-03-11 v0.1: 초기 기술 아키텍처 초안 작성
