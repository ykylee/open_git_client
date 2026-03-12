#include "application/common/task_coordinator.h"

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
    ogc::application::TaskCoordinator coordinator;

    std::string startedRepo;
    std::string startedLane;
    std::string finishedRepo;
    std::string finishedLane;
    bool executed = false;

    coordinator.setTaskStartedCallback([&](const std::string& repoPath, const std::string& lane) {
        startedRepo = repoPath;
        startedLane = lane;
    });
    coordinator.setTaskFinishedCallback([&](const std::string& repoPath, const std::string& lane) {
        finishedRepo = repoPath;
        finishedLane = lane;
    });

    coordinator.runRead("/tmp/repo", [&]() {
        executed = true;
    });

    expect(executed, "read task was not executed");
    expect(startedRepo == "/tmp/repo", "read task started repo mismatch");
    expect(startedLane == "read", "read task started lane mismatch");
    expect(finishedRepo == "/tmp/repo", "read task finished repo mismatch");
    expect(finishedLane == "read", "read task finished lane mismatch");

    executed = false;

    coordinator.runWrite("/tmp/repo", [&]() {
        executed = true;
    });

    expect(executed, "write task was not executed");
    expect(startedLane == "write", "write task started lane mismatch");
    expect(finishedLane == "write", "write task finished lane mismatch");

    return 0;
}
