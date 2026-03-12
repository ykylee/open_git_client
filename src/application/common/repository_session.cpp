#include "application/common/repository_session.h"

namespace ogc::application {

RepositorySession::RepositorySession(std::string repoPath)
    : repoPath_(std::move(repoPath)) {
}

const std::string& RepositorySession::repoPath() const {
    return repoPath_;
}

const domain::RepositorySnapshot& RepositorySession::snapshot() const {
    return snapshot_;
}

TaskCoordinator& RepositorySession::taskCoordinator() {
    return taskCoordinator_;
}

void RepositorySession::setSnapshot(domain::RepositorySnapshot snapshot) {
    snapshot_ = std::move(snapshot);
    if (snapshotChangedCallback_) {
        snapshotChangedCallback_();
    }
}

void RepositorySession::setSnapshotChangedCallback(SnapshotObserver callback) {
    snapshotChangedCallback_ = std::move(callback);
}

}  // namespace ogc::application
