# Initial Design Specification

## 문서 정보
- 문서 버전: v0.1
- 작성일: 2026-03-11
- 기준 문서: `docs/requirements/SRS.md` v0.6
- 범위: Release 1 핵심 구현

## 목적
이 문서는 현재 SRS를 구현 가능한 단위로 해석해 초기 설계 결정을 고정하는 문서다. 화면 구조, 모듈 경계, 도메인 모델, Git 처리 흐름, 구현 순서를 포함한다.

## 설계 목표
- Release 1 범위를 빠르게 구현할 수 있는 단순한 구조를 택한다.
- UI 반응성을 유지하기 위해 모든 장시간 Git 작업을 비동기 처리한다.
- 로컬 고빈도 작업은 `libgit2`로 우선 처리하고, 복잡하거나 호환성 민감한 작업은 네이티브 `git` CLI로 보완한다.
- Windows 실행을 우선하되 Linux 빌드 가능성을 해치지 않는 구조를 유지한다.

## Release 1 설계 범위
- 저장소 열기, 복제, 초기화
- 다중 저장소 전환
- 커밋 그래프와 참조 시각화
- 작업 트리, 스테이징, 부분 스테이징
- 커밋, amend, revert, reset
- undo/redo
- 브랜치 관리
- merge, rebase, 충돌 해결
- remote 관리, fetch, pull, push
- 검색, diff, file history, blame
- 대용량 저장소 대응, 비차단 처리

## 화면 정보 구조

### 메인 윈도우
- Top Toolbar
  저장소 열기, 복제, 새로고침, fetch, pull, push, 검색 진입
- Left Sidebar
  저장소 목록, 브랜치/태그/원격/stash 필터, 숨김/강조 토글
- Center Graph View
  커밋 그래프, 브랜치 라벨, 커밋 요약, 작성자/시간 정보
- Right Inspector
  선택 커밋 상세, diff, 파일 이력, blame, 브랜치/원격 상세
- Bottom Working Tree Panel
  staged / unstaged 목록, 부분 스테이징, 커밋 작성, 작업 진행 상태

### 보조 다이얼로그
- Clone Repository Dialog
- Open Repository Dialog
- Init Repository Dialog
- Branch Create / Rename Dialog
- Remote Manage Dialog
- Merge / Rebase Dialog
- Conflict Resolution Dialog
- Reset / Revert Confirmation Dialog
- Preferences Dialog

## 핵심 사용자 흐름

### 1. 저장소 열기
1. 사용자가 저장소를 선택한다.
2. 앱은 저장소 메타데이터와 기본 상태를 비동기 로딩한다.
3. 그래프, 작업 트리, 브랜치 목록을 병렬 초기화한다.
4. 초기 렌더 후 추가 이력은 점진적으로 로딩한다.

### 2. 커밋 작성
1. 작업 트리 패널에서 파일 또는 hunk를 스테이징한다.
2. 커밋 메시지를 입력한다.
3. 앱은 인덱스 상태를 검증한 뒤 commit use case를 실행한다.
4. 성공 시 그래프와 작업 트리를 부분 갱신한다.

### 3. 브랜치 전환
1. 사용자가 브랜치를 선택한다.
2. 앱은 워킹 트리 충돌 가능성을 사전 검사한다.
3. 전환 가능하면 checkout 작업을 백그라운드 실행한다.
4. 성공 시 저장소 상태를 재동기화한다.

### 4. fetch / pull / push
1. 사용자가 동기화 작업을 요청한다.
2. 앱은 원격, 인증, 현재 브랜치 상태를 확인한다.
3. 작업은 CLI backend 또는 적절한 backend로 실행한다.
4. 완료 후 그래프와 참조 목록만 선택적으로 갱신한다.

## 모듈 설계

### `app`
- `main()`
- 의존성 조립
- 애플리케이션 수명주기 관리

### `ui`
- `MainWindow`
- `RepositorySidebar`
- `CommitGraphView`
- `CommitInspectorPanel`
- `WorkingTreePanel`
- 각종 다이얼로그

### `application`
- `RepositoryUseCases`
- `CommitUseCases`
- `BranchUseCases`
- `SyncUseCases`
- `HistoryUseCases`
- `UndoRedoService`
- `TaskCoordinator`

### `domain`
- `RepositorySummary`
- `RepositoryState`
- `CommitNode`
- `ReferenceLabel`
- `ChangedFile`
- `DiffHunk`
- `BranchInfo`
- `RemoteInfo`
- `OperationResult`

### `git-core`
- `IGitRepository`
- `IGitHistoryService`
- `IGitWorkingTreeService`
- `IGitBranchService`
- `IGitSyncService`
- `IGitConflictService`

### `git-libgit2`
- 읽기 중심 구현
- 인덱스/상태 조회
- 그래프 데이터 읽기
- blame / file history / diff 일부 처리

### `git-cli`
- fetch / pull / push
- merge / rebase
- 일부 reset / revert / conflict 해결 보조
- credential helper 연동

### `platform`
- 프로세스 실행
- 파일시스템 감시
- 타이머 / 백그라운드 작업 추상화

## 도메인 모델 초안

### RepositoryState
- `repo_path`
- `head_ref`
- `head_commit`
- `current_operation`
- `is_busy`
- `last_refresh_at`

### CommitNode
- `oid`
- `short_oid`
- `summary`
- `author_name`
- `author_email`
- `author_time`
- `parents`
- `refs`

### ChangedFile
- `path`
- `git_status`
- `staged`
- `unstaged`
- `binary`

### DiffHunk
- `old_start`
- `old_count`
- `new_start`
- `new_count`
- `header`
- `lines`

## Git backend 분리 규칙

### `libgit2` 우선 작업
- repository open
- log / graph read
- branch / tag / ref read
- status read
- index read
- staged / unstaged diff
- blame
- file history

### CLI 우선 작업
- fetch
- pull
- push
- merge
- rebase
- conflict continuation / abort
- credential helper가 필요한 인증 흐름

### 선택 기준
- 프로세스 생성 비용보다 데이터 접근 빈도가 더 중요한 읽기 작업은 `libgit2`
- Git 원본 동작 호환성이 중요한 작업은 CLI
- 두 backend가 모두 가능한 작업은 먼저 `libgit2`로 설계하고, 구현 난이도나 호환성 문제가 있으면 CLI fallback을 둔다.

## 상태 관리 설계
- 저장소별로 독립된 `RepositorySession`을 둔다.
- 세션은 `graph_state`, `working_tree_state`, `selection_state`, `operation_state`를 가진다.
- UI는 세션 상태 스냅샷을 읽고, 변경은 use case를 통해서만 반영한다.
- 작업 완료 후 전체 새로고침 대신 영향 범위만 부분 갱신한다.

## 작업 실행 모델
- 모든 Git 작업은 `TaskCoordinator`를 통해 실행한다.
- 읽기 작업은 병렬 가능하다.
- 저장소 쓰기 작업은 저장소 단위 mutex 또는 직렬 큐로 제한한다.
- 같은 저장소에 대한 중복 refresh는 coalescing한다.
- 긴 작업은 progress, cancel, timeout 정보를 노출한다.

## 캐시 전략 초안
- 최근 커밋 그래프는 페이지 단위 캐시
- 브랜치/태그 목록은 저장소 상태 변경 시점 기준 캐시
- 작업 트리 상태는 debounce refresh
- blame / file history는 파일 단위 LRU 캐시

## 오류 처리 원칙
- Git 오류는 사용자 메시지와 원본 stderr/diagnostic을 분리해 저장한다.
- 치명적 오류와 사용자 조치 가능 오류를 구분한다.
- reset, discard, branch delete 같은 파괴적 작업은 확인 단계를 둔다.

## 프로젝트 디렉토리 구조 초안
```text
src/
  app/
  ui/
  application/
  domain/
  git-core/
  git-libgit2/
  git-cli/
  platform/
tests/
  unit/
  integration/
resources/
cmake/
third_party/
```

## 구현 우선순위

### Phase 1
- 프로젝트 골격
- 저장소 열기
- 기본 그래프 보기
- 작업 트리 보기

### Phase 2
- 스테이징 / 커밋
- 브랜치 관리
- diff / 커밋 상세

### Phase 3
- fetch / pull / push
- merge / rebase
- conflict 처리

### Phase 4
- 검색
- blame / file history
- 대용량 저장소 최적화
- undo / redo

## 보류 결정
- pull request UI
- submodule UI
- Git LFS UI
- cloud / team 기능

## 후속 상세 설계 필요 항목
- Commit graph widget 렌더링 방식
- hunk 단위 편집 모델
- undo/redo 명령 로그 포맷
- conflict resolution 편집 UX
- 성능 측정용 대표 저장소 세트

## 변경 이력
- 2026-03-11 v0.1: SRS 기반 초기 설계 명세 작성
