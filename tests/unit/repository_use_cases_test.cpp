#include "application/repository/repository_use_cases.h"
#include "git-core/in_memory_git_backend.h"

#include <QSignalSpy>
#include <QtTest/QtTest>

namespace ogc::tests {

class RepositoryUseCasesTest : public QObject {
    Q_OBJECT

private slots:
    void openRepositoryCreatesInitializedSession();
    void refreshRepositoryUpdatesSessionAndEmitsSignal();
};

void RepositoryUseCasesTest::openRepositoryCreatesInitializedSession() {
    git::InMemoryGitBackend backend;
    application::RepositoryUseCases useCases(backend);

    const auto session = useCases.openRepository("/tmp/open_git_client");

    QVERIFY(session != nullptr);
    QCOMPARE(session->repoPath(), QString("/tmp/open_git_client"));
    QCOMPARE(session->snapshot().summary.currentBranch, QString("codeserver"));
    QVERIFY(!session->snapshot().graph.empty());
    QVERIFY(!session->snapshot().workingTree.empty());
}

void RepositoryUseCasesTest::refreshRepositoryUpdatesSessionAndEmitsSignal() {
    git::InMemoryGitBackend backend;
    application::RepositoryUseCases useCases(backend);
    auto session = useCases.openRepository("/tmp/open_git_client");
    QSignalSpy snapshotSpy(session.get(), &application::RepositorySession::snapshotChanged);

    useCases.refreshRepository(*session);

    QCOMPARE(snapshotSpy.count(), 1);
    QCOMPARE(session->snapshot().state.currentOperation, QString("idle"));
    QCOMPARE(session->snapshot().summary.displayName, QString("open_git_client"));
}

}  // namespace ogc::tests

QTEST_MAIN(ogc::tests::RepositoryUseCasesTest)

#include "repository_use_cases_test.moc"
