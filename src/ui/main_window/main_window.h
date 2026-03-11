#pragma once

#include "application/common/repository_session.h"
#include "application/repository/repository_use_cases.h"

#include <QMainWindow>
#include <QString>

#include <memory>

QT_BEGIN_NAMESPACE
class QLabel;
class QListWidget;
class QSplitter;
class QTabWidget;
class QTextEdit;
class QTreeWidget;
QT_END_NAMESPACE

namespace ogc::ui {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(application::RepositoryUseCases& repositoryUseCases, QWidget* parent = nullptr);

private slots:
    void openRepository();
    void refreshActiveRepository();
    void syncActiveSessionToUi();

private:
    void setupUi();
    void setupToolbar();
    void populateGraph();
    void populateWorkingTree();
    void populateInspector();
    [[nodiscard]] QString defaultRepositoryPath() const;

    application::RepositoryUseCases& repositoryUseCases_;
    std::unique_ptr<application::RepositorySession> activeSession_;

    QLabel* repositoryTitle_ = nullptr;
    QLabel* repositoryStatus_ = nullptr;
    QListWidget* repositoryList_ = nullptr;
    QTreeWidget* graphView_ = nullptr;
    QTabWidget* inspectorTabs_ = nullptr;
    QTextEdit* inspectorDetails_ = nullptr;
    QTreeWidget* workingTreeView_ = nullptr;
};

}  // namespace ogc::ui
