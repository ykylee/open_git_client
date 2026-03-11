#include "application/common/task_coordinator.h"

namespace ogc::application {

TaskCoordinator::TaskCoordinator(QObject* parent)
    : QObject(parent) {
}

void TaskCoordinator::runRead(const QString& repoPath, std::function<void()> task) {
    emit taskStarted(repoPath, "read");
    task();
    emit taskFinished(repoPath, "read");
}

void TaskCoordinator::runWrite(const QString& repoPath, std::function<void()> task) {
    emit taskStarted(repoPath, "write");
    task();
    emit taskFinished(repoPath, "write");
}

}  // namespace ogc::application
