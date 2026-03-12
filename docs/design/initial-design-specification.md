# Initial Design Specification

## 문서 정보
- 문서 버전: v0.2
- 기준 개발 버전: v0.0.2
- 작성일: 2026-03-11
- 기준 문서:
  - `docs/requirements/SRS.md` v0.6
  - `docs/design/initial-technical-architecture.md` v0.2
  - `docs/design/gitkraken-feature-analysis.md` v0.1
- 범위: Release 1 초기 설계 고정

## 목적
이 문서는 현재 요구사항과 기존 기술 아키텍처 초안을 바탕으로 Release 1 구현 전에 고정해야 할 설계 결정을 정리한다. 이 문서의 목표는 "무엇을 만들지"가 아니라 "어떤 구조로 나눠서 만들지"를 합의 가능한 수준까지 구체화하는 것이다.

## 설계 입력 요약
- `SRS-014`부터 `SRS-024`까지의 Release 1 기능 범위를 구조화 대상으로 삼는다.
- `SRS-003`, `SRS-004`, `SRS-024`에 따라 성능과 비차단 처리를 상위 설계 제약으로 둔다.
- `SRS-006`부터 `SRS-013`까지의 플랫폼 및 기술 제약에 따라 `C++20 + Qt 6 + CMake + libgit2 + native git CLI`를 유지한다.
- GitKraken 분석 문서에서 정리한 핵심 화면 구조와 Git 작업 흐름을 정보 구조의 참고 축으로 사용한다.

## 설계 원칙
- UI 스레드는 Git 저장소 스캔, 대량 diff 계산, 외부 프로세스 대기를 직접 수행하지 않는다.
- 읽기 빈도가 높고 지연에 민감한 로컬 작업은 `libgit2`를 우선 사용한다.
- Git 원본 동작과의 호환성이 더 중요한 작업은 네이티브 `git` CLI를 우선 사용한다.
- 저장소 상태는 "전체 다시 읽기"보다 "영향 범위만 갱신"하는 방향으로 모델링한다.
- Release 1에서는 기능 확장성보다 핵심 Git 작업의 안정적 완결성과 응답성 확보를 우선한다.

## Release 1 범위와 비범위

### 포함 범위
- 저장소 열기, 복제, 초기화
- 다중 저장소 목록과 빠른 전환
- 커밋 그래프, 참조 라벨, 커밋 상세 조회
- 작업 트리 상태 표시, 파일 단위 스테이징, hunk 단위 부분 스테이징
- 커밋 작성, amend, revert, reset
- undo/redo 대상 Git 작업 기록 및 복구
- 브랜치 생성, 전환, 이름 변경, 삭제
- merge, rebase, 충돌 감지 및 해결 진입
- remote 관리, fetch, pull, push
- 검색, diff, file history, blame
- 대용량 저장소 대응, 점진적 로딩, 취소 가능한 백그라운드 작업

### Release 1 비범위
- pull request / merge request UI
- submodule UI
- Git LFS UI
- team / cloud / profile / AI 기능

## ADR 요약

### ADR-001 UI 계층은 `Qt Widgets`로 시작한다
- 결정: Release 1 UI는 `Qt Widgets` 기반으로 설계한다.
- 이유: 복잡한 패널 배치, 고밀도 정보 표시, Windows 데스크톱 UX 제어가 중요하고, 초기 구현 속도와 디버깅 용이성이 필요하다.
- 결과: 그래프 뷰와 diff 뷰는 커스텀 위젯 기반으로 확장하고, QML 도입은 Release 1 범위에서 제외한다.

### ADR-002 저장소 상태의 중심 단위는 `RepositorySession`이다
- 결정: 열려 있는 저장소마다 독립적인 `RepositorySession`을 생성한다.
- 이유: 다중 저장소 전환, 캐시 분리, 저장소 단위 직렬화, 작업 취소 범위 정의가 쉬워진다.
- 결과: UI는 전역 Git 상태를 직접 읽지 않고 활성 세션의 스냅샷만 구독한다.

### ADR-003 Git 실행은 이중 백엔드 전략으로 고정한다
- 결정: 읽기 중심 로컬 작업은 `libgit2`, 호환성 민감 작업은 `git` CLI를 사용한다.
- 이유: SRS의 성능 우선 요구와 실제 Git 동작 호환성 요구를 동시에 만족시키기 위한 절충안이다.
- 결과: 각 유스케이스는 단일 백엔드에만 직접 의존하지 않고 `git-core` 인터페이스를 통해 호출한다.

### ADR-004 작업 실행은 "읽기 병렬, 쓰기 직렬" 원칙을 따른다
- 결정: 읽기 작업은 병렬 풀에서 실행하고, 저장소 변경 작업은 저장소별 직렬 큐에서 실행한다.
- 이유: UI 응답성을 유지하면서도 index, HEAD, refs를 다루는 경쟁 조건을 줄일 수 있다.
- 결과: `TaskCoordinator`는 저장소별 operation lane을 가진다.

## 시스템 컨텍스트
```text
+------------------------------+
| MainWindow                   |
| - Toolbar                    |
| - Sidebar                    |
| - Graph View                 |
| - Inspector                  |
| - Working Tree Panel         |
+---------------+--------------+
                |
                v
+------------------------------+
| Presentation / View Models   |
| - ActiveRepositoryPresenter  |
| - GraphPresenter             |
| - WorkingTreePresenter       |
| - InspectorPresenter         |
+---------------+--------------+
                |
                v
+------------------------------+
| Application Use Cases        |
| - Repository                 |
| - Commit                     |
| - Branch                     |
| - Sync                       |
| - History                    |
| - Undo/Redo                  |
+---------------+--------------+
                |
                v
+------------------------------+
| RepositorySession            |
| - Snapshot Store             |
| - TaskCoordinator            |
| - Cache                      |
| - Operation Log              |
+----------+---------+---------+
           |         |
           v         v
+----------------+  +-------------------+
| libgit2        |  | native git CLI    |
| fast local I/O |  | compatible ops    |
+----------------+  +-------------------+
```

## 화면 구조와 책임

### Main Window
- `TopToolbar`
  저장소 열기, 복제, 새로고침, fetch, pull, push, 검색 진입, 진행 중 작업 요약을 표시한다.
- `RepositorySidebar`
  저장소 목록, 브랜치/태그/원격/stash 필터, 그래프 hide/solo 성격의 집중 보기 토글을 담당한다.
- `CommitGraphView`
  DAG 렌더링, 커밋 선택, 더 많은 이력 로딩, 그래프 focus 변경을 담당한다.
- `InspectorTabs`
  커밋 상세, diff, file history, blame, branch/remote detail을 탭 형태로 전환 표시한다.
- `WorkingTreePanel`
  staged/unstaged 목록, hunk 선택, 커밋 작성, amend, discard 진입, 현재 작업 상태를 표시한다.

### 보조 다이얼로그
- `CloneRepositoryDialog`
  URL, 대상 경로, 초기 브랜치/원격 설정을 받는다.
- `InitRepositoryDialog`
  신규 저장소 생성과 기본 브랜치 설정을 담당한다.
- `BranchDialog`
  생성, 이름 변경, 삭제 확인을 통합 처리한다.
- `RemoteDialog`
  remote 추가, 수정, 삭제를 담당한다.
- `IntegrationDialog`
  merge/rebase 대상 선택과 옵션 입력을 담당한다.
- `ConflictResolutionDialog`
  충돌 파일 목록과 해결 편집 진입점을 제공한다.
- `DangerousOperationDialog`
  reset, discard, branch delete 같은 파괴적 작업의 사전 확인을 담당한다.

## 핵심 런타임 모델

### `RepositorySession`
- 하나의 로컬 저장소에 대한 런타임 컨테이너다.
- 활성 저장소 전환은 "다른 세션 선택"으로 처리하고, 세션 자체를 즉시 파기하지 않는다.
- 세션은 아래 상태 조각을 보유한다.

### 세션 상태 조각
- `RepositoryDescriptor`
  저장소 경로, 표시 이름, 원격 요약, 최근 접근 시각
- `GraphState`
  현재 로드된 커밋 노드 페이지, visible ref 필터, 선택된 커밋, 검색 결과
- `WorkingTreeState`
  staged/unstaged 파일, hunk 모델, index dirty 여부, 마지막 스캔 시각
- `InspectorState`
  선택된 대상, diff 탭 상태, 파일 이력 캐시 핸들
- `OperationState`
  현재 실행 중인 Git 작업, 진행률, 취소 가능 여부, 마지막 오류
- `UndoState`
  복구 가능한 작업 로그와 redo 후보 스택

### 상태 갱신 원칙
- 모든 UI 변경은 유스케이스의 결과로만 반영한다.
- 작업 완료 후에는 `refresh policy`를 따라 필요한 상태 조각만 갱신한다.
- 파일시스템 감시 또는 외부 Git 변경 감지는 "더티 플래그 부여 -> 지연 새로고침"으로 처리한다.

## 애플리케이션 계층 설계

### 유스케이스 그룹
- `RepositoryUseCases`
  open, clone, init, switch active repository, refresh summary
- `CommitUseCases`
  stage file, unstage file, stage hunk, create commit, amend commit, revert commit, reset
- `BranchUseCases`
  create, checkout, rename, delete, list refs
- `SyncUseCases`
  remote 관리, fetch, pull, push, 인증 위임
- `HistoryUseCases`
  load graph page, search commits, show diff, file history, blame
- `WorkspaceUseCases`
  저장소 목록과 최근 열기 상태 관리
- `UndoRedoUseCases`
  복구 가능 작업 기록, undo, redo

### 명령과 조회 분리
- 쓰기 명령은 `TaskCoordinator`를 통해 실행하고 작업 로그를 남긴다.
- 읽기 조회는 세션 캐시를 우선 확인한 뒤 부족한 범위만 백그라운드 로딩한다.
- 동일 의도를 가진 연속 조회는 coalescing 대상으로 본다.

## Git 백엔드 책임 분리

| 기능군 | 우선 백엔드 | 보조 백엔드 | 설계 이유 |
| --- | --- | --- | --- |
| 저장소 열기 / 메타데이터 조회 | `libgit2` | 없음 | 프로세스 생성 없이 빠른 로컬 접근 |
| 커밋 그래프 / ref 읽기 | `libgit2` | 없음 | 대량 반복 조회 성능이 중요 |
| working tree / index 상태 | `libgit2` | 없음 | 고빈도 상태 갱신 경로 |
| 파일/커밋 diff | `libgit2` | `git` CLI | 기본은 libgit2, 호환성 문제 시 fallback |
| file history / blame | `libgit2` | `git` CLI | 읽기 성능 우선, edge case 대비 fallback 가능 |
| fetch / pull / push | `git` CLI | 없음 | 인증, remote helper, 원본 동작 호환성 우선 |
| merge / rebase | `git` CLI | 없음 | 상태 전이와 충돌 처리 호환성 우선 |
| reset / revert | `libgit2` 우선 | `git` CLI | 단순 경로는 libgit2, 복잡한 예외 시 CLI 보조 |
| conflict continue / abort | `git` CLI | 없음 | 진행 중 작업 상태 복구와 호환성 우선 |

## 주요 사용자 흐름 설계

### 1. 저장소 열기
1. 사용자가 저장소를 선택한다.
2. `RepositoryUseCases::openRepository`가 세션을 생성하거나 재활성화한다.
3. `RepositoryDescriptor`, `GraphState`, `WorkingTreeState` 초기 읽기를 병렬 실행한다.
4. 첫 화면은 기본 요약과 최근 커밋 묶음만 렌더링한다.
5. 나머지 그래프 페이지와 inspector용 상세 데이터는 지연 로딩한다.

### 2. 커밋 작성
1. 사용자가 파일 또는 hunk를 선택해 stage/unstage를 수행한다.
2. `WorkingTreeState`는 변경된 파일만 부분 갱신한다.
3. 커밋 실행 전 `HEAD`, index 상태, 메시지 유효성을 검사한다.
4. 성공 시 `GraphState`, `WorkingTreeState`, `UndoState`만 선택 갱신한다.

### 3. 브랜치 전환
1. 전환 요청 시 로컬 변경과 충돌 가능성을 사전 확인한다.
2. checkout은 저장소 쓰기 큐에 단일 작업으로 넣는다.
3. 성공 시 graph, working tree, inspector 선택 상태를 동시에 재정렬한다.
4. 실패 시 오류 유형을 "자동 복구 가능 / 사용자 조치 필요"로 나눠 노출한다.

### 4. fetch / pull / push
1. sync 요청은 CLI 백엔드에서 실행한다.
2. 인증 입력이 필요하면 platform 계층이 프로세스 상호작용을 중개한다.
3. 완료 후 graph 전체를 다시 읽지 않고 refs와 HEAD 관련 구간만 우선 갱신한다.
4. pull/rebase 이후 충돌이 생기면 `OperationState`를 충돌 모드로 전환한다.

### 5. merge / rebase / conflict
1. merge/rebase는 항상 저장소 쓰기 큐를 사용한다.
2. 시작 시 현재 operation token을 세션에 기록한다.
3. 충돌 발생 시 충돌 파일 목록과 해결 상태를 `WorkingTreeState`에 병합 반영한다.
4. continue/abort/skip 흐름은 CLI 백엔드가 소유한다.

### 6. undo / redo
1. undo/redo는 모든 Git 작업을 지원하지 않는다.
2. Release 1 대상은 `checkout`, `commit`, `discard`, `branch delete`, `remote remove`, `branch reset`이다.
3. 각 작업은 실행 시 복구 메타데이터를 `UndoState`에 남긴다.
4. 복구 불가능 작업은 사전에 명시적으로 표시한다.

## 도메인 모델 상세화

### `RepositorySummary`
- `repo_path`
- `display_name`
- `is_bare`
- `current_branch`
- `ahead_count`
- `behind_count`
- `has_uncommitted_changes`

### `RepositoryState`
- `repo_path`
- `head_ref`
- `head_commit`
- `current_operation`
- `is_busy`
- `is_conflicted`
- `last_refresh_at`

### `CommitNode`
- `oid`
- `short_oid`
- `summary`
- `author_name`
- `author_email`
- `author_time`
- `parents`
- `refs`
- `graph_lane`
- `is_merge_commit`

### `ReferenceLabel`
- `name`
- `type`
- `is_remote`
- `is_head`
- `color_key`

### `ChangedFile`
- `path`
- `git_status`
- `has_staged_changes`
- `has_unstaged_changes`
- `is_binary`
- `is_conflicted`

### `DiffHunk`
- `old_start`
- `old_count`
- `new_start`
- `new_count`
- `header`
- `lines`
- `is_selectable`

### `OperationResult`
- `status`
- `message`
- `diagnostic`
- `refresh_policy`
- `undo_metadata`

## 작업 실행 모델

### `TaskCoordinator`
- 저장소별로 하나의 write lane과 하나 이상의 read worker를 가진다.
- 같은 저장소에서 write 작업이 실행 중일 때 읽기 작업은 허용하되, 결과 일관성이 깨질 수 있는 조회는 큐 뒤로 미룬다.
- 같은 종류의 refresh 요청은 하나로 합친다.
- 사용자가 저장소를 전환하면 비활성 세션의 긴 읽기 작업은 취소 후보로 표시한다.

### 취소와 타임아웃
- 그래프 추가 로딩, 검색, blame, file history는 사용자가 취소할 수 있어야 한다.
- fetch/pull/push와 merge/rebase는 best-effort 취소로 처리한다.
- timeout은 강제 종료보다 "경고 + 사용자 선택"을 우선한다.

## 캐시 및 새로고침 전략
- `GraphState`
  최신 커밋 페이지부터 순차 캐시하고, 더 깊은 이력은 page cursor로 이어 붙인다.
- `WorkingTreeState`
  파일 변경 감지는 debounce 후 다시 읽고, 방금 변경된 파일은 우선 갱신한다.
- `ReferenceCache`
  branch/tag/remote 목록은 refs 변경 작업 이후에만 무효화한다.
- `DiffCache`
  선택된 커밋 또는 파일 기준 단기 캐시를 사용한다.
- `HistoryCache`
  file history / blame는 파일 경로 단위 LRU 캐시를 둔다.

## 대용량 저장소 대응 전략
- 초기 그래프 로딩은 최근 이력의 제한된 page만 가져온다.
- 그래프 노드와 inspector 상세 로딩을 분리해 초기 렌더를 앞당긴다.
- 검색은 전체 그래프 재구성이 아니라 별도 결과 집합으로 유지한다.
- 저장소 새로고침은 "graph", "refs", "working tree", "inspector detail" 단위로 나눠 부분 수행한다.
- 성능 기준은 구현 전에 별도 벤치마크 문서로 수치화하되, 초기 설계에서는 "UI 프리즈 금지"와 "부분 새로고침 우선"을 고정 원칙으로 둔다.

## 오류 처리와 안전장치
- 사용자 메시지와 원본 Git 진단 메시지를 분리 보관한다.
- destructive 작업은 모두 사전 확인 단계를 둔다.
- 충돌 상태, detached HEAD, remote 인증 실패는 별도 오류 분류를 가진다.
- CLI 실패 시 stderr를 그대로 UI에 노출하지 않고 사용자 친화 메시지와 세부 정보 보기로 분리한다.

## 초기 프로젝트 구조
```text
src/
  app/
  ui/
    main_window/
    sidebar/
    graph/
    inspector/
    working_tree/
    dialogs/
  application/
    repository/
    commit/
    branch/
    sync/
    history/
    undo/
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

## 설계 기준 구현 단계

### Phase 1 Foundation
- 앱 골격, `RepositorySession`, `TaskCoordinator`
- 저장소 열기/전환
- 기본 그래프 page 로딩
- working tree 상태 로딩

### Phase 2 Authoring
- stage/unstage/hunk stage
- commit/amend
- commit detail / diff inspector
- branch 생성/전환/삭제

### Phase 3 Synchronization
- remote 관리
- fetch/pull/push
- merge/rebase 진입
- 충돌 상태 감지

### Phase 4 Exploration and Recovery
- 검색
- blame / file history
- undo / redo
- 대용량 저장소 최적화 보강

## 후속 상세 설계 필요 항목
- commit graph widget 렌더링 알고리즘과 lane 계산 규칙
- hunk 선택 모델과 부분 스테이징 편집 UX
- undo/redo 메타데이터 포맷과 복구 가능 범위 검증
- conflict resolution 편집 UX와 3-way diff 표현 방식
- 성능 검증용 대표 저장소 세트와 측정 시나리오

## 변경 이력
### 개발 버전 `v0.0.2`
- 2026-03-12: 현재 프로젝트 개발 버전 `v0.0.2`에 맞춰 문서의 기준 개발 버전과 변경 이력 표기를 갱신했다.

### 개발 버전 `v0.0.1`
- 2026-03-11: SRS 기반 초기 설계 명세를 작성하고 Release 1 설계를 ADR, 세션 모델, 백엔드 책임 분리, 캐시 및 작업 실행 모델 수준까지 구체화했으며 변경 이력 표기를 개발 버전 기준으로 맞췄다.
