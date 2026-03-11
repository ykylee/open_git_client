#pragma once

#include "git-core/git_backend.h"

namespace ogc::git {

class InMemoryGitBackend final : public GitBackend {
public:
    [[nodiscard]] domain::RepositorySnapshot openRepository(const QString& repoPath) override;
    [[nodiscard]] domain::RepositorySnapshot refreshRepository(const QString& repoPath) override;

private:
    [[nodiscard]] domain::RepositorySnapshot buildSnapshot(const QString& repoPath) const;
};

}  // namespace ogc::git
