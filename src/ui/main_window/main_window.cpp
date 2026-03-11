#include "ui/main_window/main_window.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace ogc::ui {

MainWindow::MainWindow(application::RepositoryUseCases& repositoryUseCases, QWidget* parent)
    : QMainWindow(parent),
      repositoryUseCases_(repositoryUseCases) {
    setupUi();
    setupToolbar();
    openRepository();
}

void MainWindow::openRepository() {
    activeSession_ = repositoryUseCases_.openRepository(defaultRepositoryPath());
    connect(activeSession_.get(), &application::RepositorySession::snapshotChanged, this, &MainWindow::syncActiveSessionToUi);
    syncActiveSessionToUi();
}

void MainWindow::refreshActiveRepository() {
    if (!activeSession_) {
        return;
    }

    repositoryUseCases_.refreshRepository(*activeSession_);
}

void MainWindow::syncActiveSessionToUi() {
    if (!activeSession_) {
        return;
    }

    const auto& snapshot = activeSession_->snapshot();

    repositoryTitle_->setText(snapshot.summary.displayName);
    repositoryStatus_->setText(
        QString("%1 | branch %2 | refreshed %3")
            .arg(snapshot.summary.repoPath, snapshot.summary.currentBranch, snapshot.state.lastRefreshAt));

    repositoryList_->clear();
    repositoryList_->addItem(snapshot.summary.displayName);
    repositoryList_->addItem("Recent repositories");
    repositoryList_->addItem("Workspace");

    populateGraph();
    populateWorkingTree();
    populateInspector();

    statusBar()->showMessage("Repository session synchronized");
}

void MainWindow::setupUi() {
    setWindowTitle("Open Git Client");
    resize(1440, 860);

    auto* root = new QSplitter(this);
    root->setOrientation(Qt::Horizontal);
    setCentralWidget(root);

    auto* sidebar = new QWidget(root);
    auto* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(12, 12, 12, 12);
    sidebarLayout->setSpacing(8);

    repositoryTitle_ = new QLabel("No repository", sidebar);
    repositoryTitle_->setStyleSheet("font-size: 20px; font-weight: 700;");
    repositoryStatus_ = new QLabel("Open a repository session", sidebar);
    repositoryStatus_->setWordWrap(true);
    repositoryList_ = new QListWidget(sidebar);

    sidebarLayout->addWidget(repositoryTitle_);
    sidebarLayout->addWidget(repositoryStatus_);
    sidebarLayout->addWidget(repositoryList_, 1);

    auto* mainPane = new QSplitter(root);
    mainPane->setOrientation(Qt::Horizontal);

    graphView_ = new QTreeWidget(mainPane);
    graphView_->setColumnCount(4);
    graphView_->setHeaderLabels({"Graph", "Commit", "Author", "Refs"});
    graphView_->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    graphView_->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    graphView_->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    graphView_->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    auto* detailPane = new QSplitter(mainPane);
    detailPane->setOrientation(Qt::Vertical);

    inspectorTabs_ = new QTabWidget(detailPane);
    inspectorDetails_ = new QTextEdit(inspectorTabs_);
    inspectorDetails_->setReadOnly(true);
    inspectorTabs_->addTab(inspectorDetails_, "Inspector");

    auto* historyPreview = new QPlainTextEdit(inspectorTabs_);
    historyPreview->setReadOnly(true);
    historyPreview->setPlainText("History and blame panes will attach here in Phase 2.");
    inspectorTabs_->addTab(historyPreview, "History");

    workingTreeView_ = new QTreeWidget(detailPane);
    workingTreeView_->setColumnCount(4);
    workingTreeView_->setHeaderLabels({"Path", "Status", "Staged", "Conflicted"});
    workingTreeView_->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    root->addWidget(sidebar);
    root->addWidget(mainPane);
    root->setStretchFactor(0, 0);
    root->setStretchFactor(1, 1);
    mainPane->setStretchFactor(0, 3);
    mainPane->setStretchFactor(1, 2);
    detailPane->setStretchFactor(0, 3);
    detailPane->setStretchFactor(1, 2);
}

void MainWindow::setupToolbar() {
    auto* toolbar = addToolBar("Main");
    toolbar->setMovable(false);

    auto* openAction = new QAction("Open", this);
    auto* refreshAction = new QAction("Refresh", this);
    auto* fetchAction = new QAction("Fetch", this);
    auto* pullAction = new QAction("Pull", this);
    auto* pushAction = new QAction("Push", this);

    connect(openAction, &QAction::triggered, this, &MainWindow::openRepository);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshActiveRepository);

    toolbar->addAction(openAction);
    toolbar->addAction(refreshAction);
    toolbar->addSeparator();
    toolbar->addAction(fetchAction);
    toolbar->addAction(pullAction);
    toolbar->addAction(pushAction);
}

void MainWindow::populateGraph() {
    graphView_->clear();

    const auto& graph = activeSession_->snapshot().graph;
    for (const auto& commit : graph) {
        QStringList refs;
        refs.reserve(static_cast<qsizetype>(commit.refs.size()));
        for (const auto& ref : commit.refs) {
            refs << ref.name;
        }

        auto* item = new QTreeWidgetItem(graphView_);
        item->setText(0, QString("| %1").arg(commit.graphLane));
        item->setText(1, QString("%1  %2").arg(commit.shortOid, commit.summary));
        item->setText(2, commit.authorName);
        item->setText(3, refs.join(", "));
    }
}

void MainWindow::populateWorkingTree() {
    workingTreeView_->clear();

    const auto& workingTree = activeSession_->snapshot().workingTree;
    for (const auto& file : workingTree) {
        auto* item = new QTreeWidgetItem(workingTreeView_);
        item->setText(0, file.path);
        item->setText(1, file.gitStatus);
        item->setText(2, file.hasStagedChanges ? "yes" : "no");
        item->setText(3, file.isConflicted ? "yes" : "no");
    }
}

void MainWindow::populateInspector() {
    const auto& snapshot = activeSession_->snapshot();
    inspectorDetails_->setPlainText(
        QString("RepositorySession\n"
                "Path: %1\n"
                "HEAD: %2\n"
                "Commit: %3\n"
                "Operation: %4\n"
                "Working tree dirty: %5\n")
            .arg(snapshot.summary.repoPath,
                 snapshot.state.headRef,
                 snapshot.state.headCommit,
                 snapshot.state.currentOperation,
                 snapshot.summary.hasUncommittedChanges ? "yes" : "no"));
}

QString MainWindow::defaultRepositoryPath() const {
    return QDir::currentPath();
}

}  // namespace ogc::ui
