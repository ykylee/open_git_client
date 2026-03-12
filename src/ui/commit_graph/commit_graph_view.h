#pragma once

#include "domain/models.h"

#include <wx/scrolwin.h>

#include <vector>

namespace ogc::ui {

class CommitGraphView final : public wxScrolledWindow {
public:
    explicit CommitGraphView(wxWindow* parent);

    void setGraph(std::vector<domain::CommitNode> graph);

private:
    void updateVirtualSize();
    void paintEvent(wxPaintEvent& event);
    void sizeEvent(wxSizeEvent& event);

    [[nodiscard]] int graphWidth() const;
    [[nodiscard]] int totalHeight() const;

    std::vector<domain::CommitNode> graph_;
};

}  // namespace ogc::ui
