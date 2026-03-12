#include "git-core/in_memory_git_backend.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace ogc::git {

namespace {

std::string currentUtcTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm utcTime {};
#if defined(_WIN32)
    gmtime_s(&utcTime, &time);
#else
    gmtime_r(&time, &utcTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

domain::CommitNode makeCommitNode(
    std::string oid,
    std::string shortOid,
    std::string summary,
    std::string authorTime,
    std::vector<domain::ReferenceLabel> refs,
    int graphLane,
    bool isMergeCommit = false) {
    domain::CommitNode node;
    node.oid = std::move(oid);
    node.shortOid = std::move(shortOid);
    node.summary = std::move(summary);
    node.authorName = "Open Git Client";
    node.authorEmail = "dev@local";
    node.authorTime = std::move(authorTime);
    node.refs = std::move(refs);
    node.graphLane = graphLane;
    node.isMergeCommit = isMergeCommit;
    return node;
}

}  // namespace

domain::RepositorySnapshot InMemoryGitBackend::openRepository(const std::string& repoPath) {
    return buildSnapshot(repoPath);
}

domain::RepositorySnapshot InMemoryGitBackend::refreshRepository(const std::string& repoPath) {
    return buildSnapshot(repoPath);
}

domain::RepositorySnapshot InMemoryGitBackend::buildSnapshot(const std::string& repoPath) const {
    domain::RepositorySnapshot snapshot;

    const std::filesystem::path path(repoPath);
    const std::string displayName = path.filename().empty() ? repoPath : path.filename().string();
    const std::string refreshedAt = currentUtcTimestamp();

    snapshot.summary.repoPath = repoPath;
    snapshot.summary.displayName = displayName;
    snapshot.summary.currentBranch = "codeserver";
    snapshot.summary.hasUncommittedChanges = true;

    snapshot.state.repoPath = repoPath;
    snapshot.state.headRef = "refs/heads/codeserver";
    snapshot.state.headCommit = "3d4f7ac";
    snapshot.state.currentOperation = "idle";
    snapshot.state.lastRefreshAt = refreshedAt;

    snapshot.graph = {
        makeCommitNode(
            "3d4f7ac45b5df741778f5ec2e6c7f9a8877dcb12",
            "3d4f7ac",
            "Bootstrap wxWidgets application shell",
            refreshedAt,
            {domain::ReferenceLabel{"HEAD", "head", false, true, "accent"},
             domain::ReferenceLabel{"codeserver", "branch", false, false, "primary"}},
            0),
        makeCommitNode(
            "024f00174bc0c4d5c7d278f31f3812fc6ffd1172",
            "024f001",
            "Import initial design documents",
            refreshedAt,
            {domain::ReferenceLabel{"origin/dev", "remote", true, false, "neutral"}},
            0)
    };

    snapshot.workingTree = {
        domain::ChangedFile{"src/app/main.cpp", "added", true, false, false, false},
        domain::ChangedFile{"src/ui/main_window/main_window.cpp", "modified", false, true, false, false},
        domain::ChangedFile{"docs/design/initial-design-specification.md", "modified", false, true, false, false}
    };

    return snapshot;
}

}  // namespace ogc::git
