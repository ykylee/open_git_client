#pragma once

#include "git-core/git_backend.h"

namespace ogc::git {

class InMemoryGitBackend final : public GitBackend {
public:
    [[nodiscard]] domain::RepositorySnapshot openRepository(const std::string& repoPath) override;
    [[nodiscard]] domain::RepositorySnapshot refreshRepository(const std::string& repoPath) override;

private:
    [[nodiscard]] domain::RepositorySnapshot buildSnapshot(const std::string& repoPath) const;
};

}  // namespace ogc::git
