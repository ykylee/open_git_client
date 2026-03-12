#pragma once

#include "application/common/repository_session.h"
#include "git-core/git_backend.h"

#include <memory>
#include <string>

namespace ogc::application {

class RepositoryUseCases final {
public:
    explicit RepositoryUseCases(git::GitBackend& gitBackend);

    [[nodiscard]] std::unique_ptr<RepositorySession> openRepository(const std::string& repoPath) const;
    void refreshRepository(RepositorySession& session) const;

private:
    git::GitBackend& gitBackend_;
};

}  // namespace ogc::application
