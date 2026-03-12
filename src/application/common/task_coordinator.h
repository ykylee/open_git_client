#pragma once

#include <functional>
#include <string>

namespace ogc::application {

class TaskCoordinator final {
public:
    using TaskObserver = std::function<void(const std::string&, const std::string&)>;

    void setTaskStartedCallback(TaskObserver callback);
    void setTaskFinishedCallback(TaskObserver callback);

    void runRead(const std::string& repoPath, std::function<void()> task);
    void runWrite(const std::string& repoPath, std::function<void()> task);

private:
    TaskObserver taskStartedCallback_;
    TaskObserver taskFinishedCallback_;
};

}  // namespace ogc::application
