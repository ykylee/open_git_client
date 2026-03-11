#include "application/common/task_coordinator.h"

#include <QSignalSpy>
#include <QtTest/QtTest>

namespace ogc::tests {

class TaskCoordinatorTest : public QObject {
    Q_OBJECT

private slots:
    void runReadEmitsLifecycleSignals();
    void runWriteEmitsLifecycleSignals();
};

void TaskCoordinatorTest::runReadEmitsLifecycleSignals() {
    application::TaskCoordinator coordinator;
    QSignalSpy startedSpy(&coordinator, &application::TaskCoordinator::taskStarted);
    QSignalSpy finishedSpy(&coordinator, &application::TaskCoordinator::taskFinished);
    bool executed = false;

    coordinator.runRead("/tmp/repo", [&]() {
        executed = true;
    });

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QVERIFY(executed);
    QCOMPARE(startedSpy.at(0).at(0).toString(), QString("/tmp/repo"));
    QCOMPARE(startedSpy.at(0).at(1).toString(), QString("read"));
    QCOMPARE(finishedSpy.at(0).at(1).toString(), QString("read"));
}

void TaskCoordinatorTest::runWriteEmitsLifecycleSignals() {
    application::TaskCoordinator coordinator;
    QSignalSpy startedSpy(&coordinator, &application::TaskCoordinator::taskStarted);
    QSignalSpy finishedSpy(&coordinator, &application::TaskCoordinator::taskFinished);
    bool executed = false;

    coordinator.runWrite("/tmp/repo", [&]() {
        executed = true;
    });

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QVERIFY(executed);
    QCOMPARE(startedSpy.at(0).at(1).toString(), QString("write"));
    QCOMPARE(finishedSpy.at(0).at(1).toString(), QString("write"));
}

}  // namespace ogc::tests

QTEST_MAIN(ogc::tests::TaskCoordinatorTest)

#include "task_coordinator_test.moc"
