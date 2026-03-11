#include "application/common/repository_session.h"

namespace ogc::application {

RepositorySession::RepositorySession(QString repoPath, QObject* parent)
    : QObject(parent),
      repoPath_(std::move(repoPath)),
      taskCoordinator_(this) {
}

const QString& RepositorySession::repoPath() const {
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
    emit snapshotChanged();
}

}  // namespace ogc::application
