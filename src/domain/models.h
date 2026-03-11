#pragma once

#include <QString>

#include <optional>
#include <vector>

namespace ogc::domain {

struct RepositorySummary {
    QString repoPath;
    QString displayName;
    bool isBare = false;
    QString currentBranch;
    int aheadCount = 0;
    int behindCount = 0;
    bool hasUncommittedChanges = false;
};

struct RepositoryState {
    QString repoPath;
    QString headRef;
    QString headCommit;
    QString currentOperation;
    bool isBusy = false;
    bool isConflicted = false;
    QString lastRefreshAt;
};

struct ReferenceLabel {
    QString name;
    QString type;
    bool isRemote = false;
    bool isHead = false;
    QString colorKey;
};

struct CommitNode {
    QString oid;
    QString shortOid;
    QString summary;
    QString authorName;
    QString authorEmail;
    QString authorTime;
    std::vector<QString> parents;
    std::vector<ReferenceLabel> refs;
    int graphLane = 0;
    bool isMergeCommit = false;
};

struct ChangedFile {
    QString path;
    QString gitStatus;
    bool hasStagedChanges = false;
    bool hasUnstagedChanges = false;
    bool isBinary = false;
    bool isConflicted = false;
};

struct OperationResult {
    enum class Status {
        Success,
        Failed
    };

    Status status = Status::Success;
    QString message;
    QString diagnostic;
    std::vector<QString> refreshPolicy;
    std::optional<QString> undoMetadata;
};

struct RepositorySnapshot {
    RepositorySummary summary;
    RepositoryState state;
    std::vector<CommitNode> graph;
    std::vector<ChangedFile> workingTree;
};

}  // namespace ogc::domain
