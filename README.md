# Open Git Client

Windows를 주요 타겟으로 하는 고성능 Git 클라이언트 프로젝트다. Linux에서도 동일 소스 트리를 빌드할 수 있도록 유지하며, Git 작업 속도와 UI 반응성을 최우선 목표로 둔다.

## 현재 상태
- 현재 개발 버전: `v0.0.1`
- 현재 단계: 요구사항 정의 및 초기 설계 정리
- 구현 상태: 애플리케이션 코드는 아직 시작되지 않았고, 문서 중심으로 프로젝트 범위를 구체화하는 중

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

## 버전 정책
- 현재 기준 개발 버전은 `v0.0.1`이다.
- 버전 번호는 사용자 지시가 있을 때만 올린다.
- 모든 문서의 변경 이력은 프로젝트 개발 버전을 기준으로 기록한다.
- 버전이 올라갈 때마다 아래 변경 이력을 함께 갱신한다.

## 변경 이력

### `v0.0.1` (개발 중)
- 2026-03-11: 프로젝트 개요와 버전 정책을 정리하고, `CRS`, `SRS`, 초기 설계 명세, 초기 기술 아키텍처, `GitKraken` 기능 분석, 스레드 운영 문서를 작성했으며 모든 문서의 변경 이력을 개발 버전 기준으로 통일했다.
