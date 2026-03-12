#pragma once

#include "application/common/repository_session.h"
#include "application/repository/repository_use_cases.h"

#include <wx/frame.h>

#include <memory>
#include <string>

class wxListBox;
class wxNotebook;
class wxStaticText;
class wxTextCtrl;
class wxTreeCtrl;

namespace ogc::ui {

class MainWindow final : public wxFrame {
public:
    explicit MainWindow(application::RepositoryUseCases& repositoryUseCases);

private:
    void openRepository();
    void refreshActiveRepository();
    void syncActiveSessionToUi();
    void setupUi();
    void setupToolbar();
    void populateGraph();
    void populateWorkingTree();
    void populateInspector();
    [[nodiscard]] std::string defaultRepositoryPath() const;

    application::RepositoryUseCases& repositoryUseCases_;
    std::unique_ptr<application::RepositorySession> activeSession_;

    wxStaticText* repositoryTitle_ = nullptr;
    wxStaticText* repositoryStatus_ = nullptr;
    wxListBox* repositoryList_ = nullptr;
    wxTreeCtrl* graphView_ = nullptr;
    wxNotebook* inspectorTabs_ = nullptr;
    wxTextCtrl* inspectorDetails_ = nullptr;
    wxTreeCtrl* workingTreeView_ = nullptr;
};

}  // namespace ogc::ui
