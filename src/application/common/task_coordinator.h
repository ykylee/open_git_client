#pragma once

#include <QObject>
#include <QString>

#include <functional>

namespace ogc::application {

class TaskCoordinator final : public QObject {
    Q_OBJECT

public:
    explicit TaskCoordinator(QObject* parent = nullptr);

    void runRead(const QString& repoPath, std::function<void()> task);
    void runWrite(const QString& repoPath, std::function<void()> task);

signals:
    void taskStarted(const QString& repoPath, const QString& lane);
    void taskFinished(const QString& repoPath, const QString& lane);
};

}  // namespace ogc::application
