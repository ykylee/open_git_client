#pragma once

#include "application/common/task_coordinator.h"
#include "domain/models.h"

#include <QObject>
#include <QString>

namespace ogc::application {

class RepositorySession final : public QObject {
    Q_OBJECT

public:
    explicit RepositorySession(QString repoPath, QObject* parent = nullptr);

    [[nodiscard]] const QString& repoPath() const;
    [[nodiscard]] const domain::RepositorySnapshot& snapshot() const;
    [[nodiscard]] TaskCoordinator& taskCoordinator();

    void setSnapshot(domain::RepositorySnapshot snapshot);

signals:
    void snapshotChanged();

private:
    QString repoPath_;
    domain::RepositorySnapshot snapshot_;
    TaskCoordinator taskCoordinator_;
};

}  // namespace ogc::application
