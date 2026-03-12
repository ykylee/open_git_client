#pragma once

#include "domain/models.h"

#include <string>

namespace ogc::git {

class GitBackend {
public:
    virtual ~GitBackend() = default;

    [[nodiscard]] virtual domain::RepositorySnapshot openRepository(const std::string& repoPath) = 0;
    [[nodiscard]] virtual domain::RepositorySnapshot refreshRepository(const std::string& repoPath) = 0;
};

}  // namespace ogc::git
