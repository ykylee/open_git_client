#pragma once

#include "domain/models.h"

#include <QString>

namespace ogc::git {

class GitBackend {
public:
    virtual ~GitBackend() = default;

    [[nodiscard]] virtual domain::RepositorySnapshot openRepository(const QString& repoPath) = 0;
    [[nodiscard]] virtual domain::RepositorySnapshot refreshRepository(const QString& repoPath) = 0;
};

}  // namespace ogc::git
