#include "application/common/task_coordinator.h"

namespace ogc::application {

void TaskCoordinator::setTaskStartedCallback(TaskObserver callback) {
    taskStartedCallback_ = std::move(callback);
}

void TaskCoordinator::setTaskFinishedCallback(TaskObserver callback) {
    taskFinishedCallback_ = std::move(callback);
}

void TaskCoordinator::runRead(const std::string& repoPath, std::function<void()> task) {
    if (taskStartedCallback_) {
        taskStartedCallback_(repoPath, "read");
    }
    task();
    if (taskFinishedCallback_) {
        taskFinishedCallback_(repoPath, "read");
    }
}

void TaskCoordinator::runWrite(const std::string& repoPath, std::function<void()> task) {
    if (taskStartedCallback_) {
        taskStartedCallback_(repoPath, "write");
    }
    task();
    if (taskFinishedCallback_) {
        taskFinishedCallback_(repoPath, "write");
    }
}

}  // namespace ogc::application
