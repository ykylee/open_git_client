#pragma once

#include "application/repository/repository_use_cases.h"
#include "git-core/in_memory_git_backend.h"

namespace ogc::app {

class ApplicationBootstrap final {
public:
    ApplicationBootstrap();

    [[nodiscard]] application::RepositoryUseCases& repositoryUseCases();

private:
    git::InMemoryGitBackend gitBackend_;
    application::RepositoryUseCases repositoryUseCases_;
};

}  // namespace ogc::app
