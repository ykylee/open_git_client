#include "app/application_bootstrap.h"
#include "ui/main_window/main_window.h"

#include <wx/app.h>

#include <memory>
#include <string_view>

namespace {

class OpenGitClientApp final : public wxApp {
public:
    bool OnInit() override {
        bootstrap_ = std::make_unique<ogc::app::ApplicationBootstrap>();
        mainWindow_ = std::make_unique<ogc::ui::MainWindow>(bootstrap_->repositoryUseCases());
        mainWindow_->Show(true);

        for (int index = 1; index < argc; ++index) {
            if (std::string_view(argv[index].ToUTF8()) == "--smoke-test") {
                CallAfter([this]() {
                    ExitMainLoop();
                });
                break;
            }
        }

        return true;
    }

private:
    std::unique_ptr<ogc::app::ApplicationBootstrap> bootstrap_;
    std::unique_ptr<ogc::ui::MainWindow> mainWindow_;
};

}  // namespace

wxIMPLEMENT_APP(OpenGitClientApp);
