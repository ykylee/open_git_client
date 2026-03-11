# GitKraken Feature Analysis

## 문서 정보
- 문서 버전: v0.1
- 기준 개발 버전: v0.0.1
- 작성일: 2026-03-11
- 목적: GitKraken 공식 문서를 참고해 기능군을 분석하고 현재 제품의 구현 범위를 정의한다.

## 분석 범위
- GitKraken Desktop 공식 가이드와 기능 문서를 기준으로 데스크톱 Git 클라이언트 기능군을 정리했다.
- 아래 우선순위 분류는 현재 요구사항인 Windows 대상, Linux 빌드 가능, Git 속도 우선 조건을 바탕으로 한 해석이다.

## 참고한 공식 문서
- [Interface Guide](https://support.gitkraken.com/start-here/interface/)
- [Open, Clone, or Initialize Repo](https://support.gitkraken.com/gitkraken-desktop/open-clone-init/)
- [Stage, Unstage, and Ignore Files](https://support.gitkraken.com/working-with-commits/staging/)
- [Committing Changes](https://support.gitkraken.com/working-with-files/commits)
- [Undo and Redo Git Actions](https://support.gitkraken.com/working-with-commits/undo-and-redo/)
- [Branch, Merge, and Rebase](https://support.gitkraken.com/working-with-repositories/branching-and-merging/)
- [Pull Requests](https://support.gitkraken.com/working-with-repositories/pull-requests/)
- [Search Git Commits](https://support.gitkraken.com/working-with-commits/search/)
- [View Diffs, File History, and Blame](https://support.gitkraken.com/gitkraken-desktop/diff/)
- [Hide and Solo](https://support.gitkraken.com/gitkraken-desktop/hiding-and-soloing/)
- [Submodules](https://support.gitkraken.com/working-with-repositories/submodules/)
- [Git LFS](https://support.gitkraken.com/gitkraken-client/git-lfs/)
- [Profiles](https://support.gitkraken.com/start-here/profiles)

## 공식 문서에서 확인된 기능군

### 1. 저장소 관리
- 로컬 저장소 열기, 원격 저장소 복제, 신규 저장소 초기화
- 여러 저장소와 탭 기반 전환
- 즐겨찾기 및 작업공간 개념

### 2. 시각적 이력 탐색
- Left Panel, Commit Graph, Commit Panel 구조
- 브랜치/태그/원격 참조 표시
- 커밋 그래프 기반 탐색과 선택

### 3. 작업 트리와 커밋 작성
- 스테이징/언스테이징
- 선택적 라인 스테이징
- 커밋 작성, amend, revert, reset
- undo/redo를 통한 안전 장치

### 4. 브랜치와 통합 작업
- 브랜치 생성, checkout, rename, delete
- merge, rebase
- 충돌 탐지와 해결 흐름

### 5. 원격 저장소 작업
- remote 추가
- fetch, pull, push
- pull 전략 선택

### 6. 탐색 보조 기능
- 커밋 검색
- 브랜치 hide/solo
- diff, file history, blame

### 7. 확장 기능
- pull request / merge request
- submodule
- Git LFS
- profile
- team view
- GitHub Actions

## 현재 제품에 대한 권장 구현 범위

### Release 1 핵심 구현
- 저장소 열기, 복제, 초기화
- 다중 저장소 전환
- 커밋 그래프와 참조 시각화
- 작업 트리 보기, 파일/라인 단위 스테이징
- 커밋, amend, revert, reset
- undo/redo
- 브랜치 생성/전환/이름변경/삭제
- merge, rebase, 충돌 해결
- remote 관리, fetch/pull/push
- 커밋 검색, 그래프 필터링, diff, file history, blame
- 대용량 저장소 대응과 비차단 처리

### Release 2 확장 구현
- GitHub/GitLab/Bitbucket/Azure DevOps pull request 연동
- submodule 지원
- Git LFS 지원

### 현재 단계에서 보류
- Team View 같은 조직 기반 협업 기능
- GitHub Actions 편집 UI
- 구독/클라우드 의존성이 큰 협업 기능
- AI 기반 기능
- Workspaces와 Profiles의 고급 운영 기능

## 판단 근거
- 위 보류 항목은 GitKraken에서 제공하지만, 현재 제품 요구사항의 핵심은 빠른 Git 작업 처리와 Windows 대상 로컬 클라이언트 제공이다.
- 따라서 초기 구현은 로컬 저장소 조작과 시각적 Git 작업 흐름에 집중하는 편이 요구사항 적합성이 높다.
- 이 우선순위 판단은 GitKraken 문서의 사실을 그대로 옮긴 것이 아니라, 현재 프로젝트 요구사항을 기준으로 재구성한 결과다.

## SRS 반영 결과
- Release 1 핵심 구현 범위는 `SRS-014`부터 `SRS-024`에 반영했다.
- Release 2 확장 구현 범위는 `SRS-025`부터 `SRS-027`에 반영했다.

## 변경 이력
### 개발 버전 `v0.0.1`
- 2026-03-11: GitKraken 공식 문서를 바탕으로 기능군과 Release 1, Release 2 구현 우선순위를 분석하고 변경 이력 표기를 개발 버전 기준으로 맞췄다.
