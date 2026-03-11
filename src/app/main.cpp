#include "app/application_bootstrap.h"
#include "ui/main_window/main_window.h"

#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);

    ogc::app::ApplicationBootstrap bootstrap;
    ogc::ui::MainWindow mainWindow(bootstrap.repositoryUseCases());
    mainWindow.show();

    if (application.arguments().contains("--smoke-test")) {
        QTimer::singleShot(250, &application, &QCoreApplication::quit);
    }

    return QApplication::exec();
}
