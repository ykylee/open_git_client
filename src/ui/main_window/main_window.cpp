#include "ui/main_window/main_window.h"

#include <wx/artprov.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#include <wx/utils.h>

#include <filesystem>
#include <utility>

namespace ogc::ui {

namespace {

wxString toWxString(const std::string& value) {
    return wxString::FromUTF8(value);
}

enum class CommandId {
    OpenRepository = wxID_HIGHEST + 1,
    RefreshRepository,
    Fetch,
    Pull,
    Push
};

}  // namespace

MainWindow::MainWindow(application::RepositoryUseCases& repositoryUseCases)
    : wxFrame(nullptr, wxID_ANY, "Open Git Client", wxDefaultPosition, wxSize(1440, 860)),
      repositoryUseCases_(repositoryUseCases) {
    setupUi();
    setupToolbar();
    openRepository();
}

void MainWindow::openRepository() {
    activeSession_ = repositoryUseCases_.openRepository(defaultRepositoryPath());
    activeSession_->setSnapshotChangedCallback([this]() {
        syncActiveSessionToUi();
    });
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

    repositoryTitle_->SetLabel(toWxString(snapshot.summary.displayName));
    repositoryStatus_->SetLabel(
        toWxString(snapshot.summary.repoPath + " | branch " + snapshot.summary.currentBranch +
                   " | refreshed " + snapshot.state.lastRefreshAt));

    repositoryList_->Clear();
    repositoryList_->Append(toWxString(snapshot.summary.displayName));
    repositoryList_->Append("Recent repositories");
    repositoryList_->Append("Workspace");

    populateGraph();
    populateWorkingTree();
    populateInspector();

    SetStatusText("Repository session synchronized");
}

void MainWindow::setupUi() {
    auto* root = new wxSplitterWindow(this, wxID_ANY);
    auto* sidebar = new wxPanel(root, wxID_ANY);
    auto* mainPane = new wxSplitterWindow(root, wxID_ANY);
    auto* detailPane = new wxSplitterWindow(mainPane, wxID_ANY);

    auto* sidebarSizer = new wxBoxSizer(wxVERTICAL);
    repositoryTitle_ = new wxStaticText(sidebar, wxID_ANY, "No repository");
    repositoryStatus_ = new wxStaticText(sidebar, wxID_ANY, "Open a repository session");
    repositoryList_ = new wxListBox(sidebar, wxID_ANY);

    repositoryTitle_->SetFont(wxFontInfo(16).Bold());
    sidebarSizer->Add(repositoryTitle_, 0, wxALL, 12);
    sidebarSizer->Add(repositoryStatus_, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 12);
    sidebarSizer->Add(repositoryList_, 1, wxALL | wxEXPAND, 12);
    sidebar->SetSizer(sidebarSizer);

    graphView_ = new CommitGraphView(mainPane);

    auto* inspectorPanel = new wxPanel(detailPane, wxID_ANY);
    auto* inspectorSizer = new wxBoxSizer(wxVERTICAL);
    inspectorTabs_ = new wxNotebook(inspectorPanel, wxID_ANY);
    inspectorDetails_ = new wxTextCtrl(
        inspectorTabs_, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY);
    auto* historyPreview = new wxTextCtrl(
        inspectorTabs_, wxID_ANY, "History and blame panes will attach here in Phase 2.",
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

    inspectorTabs_->AddPage(inspectorDetails_, "Inspector", true);
    inspectorTabs_->AddPage(historyPreview, "History", false);
    inspectorSizer->Add(inspectorTabs_, 1, wxEXPAND | wxALL, 0);
    inspectorPanel->SetSizer(inspectorSizer);

    workingTreeView_ = new wxTreeCtrl(
        detailPane, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_DEFAULT_STYLE);

    detailPane->SplitHorizontally(inspectorPanel, workingTreeView_, 480);
    mainPane->SplitVertically(graphView_, detailPane, 820);
    root->SplitVertically(sidebar, mainPane, 280);

    CreateStatusBar();
}

void MainWindow::setupToolbar() {
    auto* toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_FLAT | wxTB_TEXT);
    toolbar->AddTool(static_cast<int>(CommandId::OpenRepository), "Open",
                     wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_TOOLBAR));
    toolbar->AddTool(static_cast<int>(CommandId::RefreshRepository), "Refresh",
                     wxArtProvider::GetBitmapBundle(wxART_REDO, wxART_TOOLBAR));
    toolbar->AddSeparator();
    toolbar->AddTool(static_cast<int>(CommandId::Fetch), "Fetch",
                     wxArtProvider::GetBitmapBundle(wxART_GO_DOWN, wxART_TOOLBAR));
    toolbar->AddTool(static_cast<int>(CommandId::Pull), "Pull",
                     wxArtProvider::GetBitmapBundle(wxART_GO_BACK, wxART_TOOLBAR));
    toolbar->AddTool(static_cast<int>(CommandId::Push), "Push",
                     wxArtProvider::GetBitmapBundle(wxART_GO_FORWARD, wxART_TOOLBAR));
    toolbar->Realize();

    Bind(wxEVT_TOOL, [this](wxCommandEvent&) { openRepository(); }, static_cast<int>(CommandId::OpenRepository));
    Bind(wxEVT_TOOL, [this](wxCommandEvent&) { refreshActiveRepository(); },
         static_cast<int>(CommandId::RefreshRepository));
}

void MainWindow::populateGraph() {
    graphView_->setGraph(activeSession_->snapshot().graph);
}

void MainWindow::populateWorkingTree() {
    workingTreeView_->DeleteAllItems();
    const auto root = workingTreeView_->AddRoot("Working Tree");

    const auto& workingTree = activeSession_->snapshot().workingTree;
    for (const auto& file : workingTree) {
        const auto item = workingTreeView_->AppendItem(root, toWxString(file.path));
        workingTreeView_->AppendItem(item, toWxString("Status: " + file.gitStatus));
        workingTreeView_->AppendItem(item, file.hasStagedChanges ? "Staged: yes" : "Staged: no");
        workingTreeView_->AppendItem(item, file.isConflicted ? "Conflicted: yes" : "Conflicted: no");
    }

    workingTreeView_->ExpandAll();
}

void MainWindow::populateInspector() {
    const auto& snapshot = activeSession_->snapshot();
    const std::string details =
        "RepositorySession\n"
        "Path: " + snapshot.summary.repoPath + "\n"
        "HEAD: " + snapshot.state.headRef + "\n"
        "Commit: " + snapshot.state.headCommit + "\n"
        "Operation: " + snapshot.state.currentOperation + "\n"
        "Working tree dirty: " + std::string(snapshot.summary.hasUncommittedChanges ? "yes" : "no") + "\n";
    inspectorDetails_->SetValue(toWxString(details));
}

std::string MainWindow::defaultRepositoryPath() const {
    return std::filesystem::current_path().string();
}

}  // namespace ogc::ui
