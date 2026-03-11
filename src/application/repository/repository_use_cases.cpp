#include "application/repository/repository_use_cases.h"

namespace ogc::application {

RepositoryUseCases::RepositoryUseCases(git::GitBackend& gitBackend)
    : gitBackend_(gitBackend) {
}

std::unique_ptr<RepositorySession> RepositoryUseCases::openRepository(const QString& repoPath) const {
    auto session = std::make_unique<RepositorySession>(repoPath);
    session->taskCoordinator().runRead(repoPath, [&]() {
        session->setSnapshot(gitBackend_.openRepository(repoPath));
    });
    return session;
}

void RepositoryUseCases::refreshRepository(RepositorySession& session) const {
    session.taskCoordinator().runRead(session.repoPath(), [&]() {
        session.setSnapshot(gitBackend_.refreshRepository(session.repoPath()));
    });
}

}  // namespace ogc::application
