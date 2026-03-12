#include "application/repository/repository_use_cases.h"
#include "git-core/in_memory_git_backend.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

}  // namespace

int main() {
    ogc::git::InMemoryGitBackend backend;
    ogc::application::RepositoryUseCases useCases(backend);

    const auto session = useCases.openRepository("/tmp/open_git_client");

    expect(session != nullptr, "session was not created");
    expect(session->repoPath() == "/tmp/open_git_client", "repo path mismatch");
    expect(session->snapshot().summary.currentBranch == "codeserver", "branch mismatch");
    expect(!session->snapshot().graph.empty(), "graph should not be empty");
    expect(!session->snapshot().workingTree.empty(), "working tree should not be empty");

    bool snapshotChanged = false;
    auto refreshSession = useCases.openRepository("/tmp/open_git_client");
    refreshSession->setSnapshotChangedCallback([&]() {
        snapshotChanged = true;
    });

    useCases.refreshRepository(*refreshSession);

    expect(snapshotChanged, "refresh did not report snapshot update");
    expect(refreshSession->snapshot().state.currentOperation == "idle", "operation state mismatch");
    expect(refreshSession->snapshot().summary.displayName == "open_git_client", "display name mismatch");

    return 0;
}
