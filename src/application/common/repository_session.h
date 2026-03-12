#pragma once

#include "application/common/task_coordinator.h"
#include "domain/models.h"

#include <functional>
#include <string>

namespace ogc::application {

class RepositorySession final {
public:
    using SnapshotObserver = std::function<void()>;

    explicit RepositorySession(std::string repoPath);

    [[nodiscard]] const std::string& repoPath() const;
    [[nodiscard]] const domain::RepositorySnapshot& snapshot() const;
    [[nodiscard]] TaskCoordinator& taskCoordinator();

    void setSnapshot(domain::RepositorySnapshot snapshot);
    void setSnapshotChangedCallback(SnapshotObserver callback);

private:
    std::string repoPath_;
    domain::RepositorySnapshot snapshot_;
    TaskCoordinator taskCoordinator_;
    SnapshotObserver snapshotChangedCallback_;
};

}  // namespace ogc::application
