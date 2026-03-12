#pragma once

#include <optional>
#include <string>
#include <vector>

namespace ogc::domain {

struct RepositorySummary {
    std::string repoPath;
    std::string displayName;
    bool isBare = false;
    std::string currentBranch;
    int aheadCount = 0;
    int behindCount = 0;
    bool hasUncommittedChanges = false;
};

struct RepositoryState {
    std::string repoPath;
    std::string headRef;
    std::string headCommit;
    std::string currentOperation;
    bool isBusy = false;
    bool isConflicted = false;
    std::string lastRefreshAt;
};

struct ReferenceLabel {
    std::string name;
    std::string type;
    bool isRemote = false;
    bool isHead = false;
    std::string colorKey;
};

struct CommitNode {
    std::string oid;
    std::string shortOid;
    std::string summary;
    std::string authorName;
    std::string authorEmail;
    std::string authorTime;
    std::vector<std::string> parents;
    std::vector<ReferenceLabel> refs;
    int graphLane = 0;
    bool isMergeCommit = false;
};

struct ChangedFile {
    std::string path;
    std::string gitStatus;
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
    std::string message;
    std::string diagnostic;
    std::vector<std::string> refreshPolicy;
    std::optional<std::string> undoMetadata;
};

struct RepositorySnapshot {
    RepositorySummary summary;
    RepositoryState state;
    std::vector<CommitNode> graph;
    std::vector<ChangedFile> workingTree;
};

}  // namespace ogc::domain
