#include "ui/commit_graph/commit_graph_view.h"

#include <wx/dcbuffer.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/pen.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <utility>

namespace ogc::ui {

namespace {

constexpr int kLeftPadding = 24;
constexpr int kTopPadding = 20;
constexpr int kBottomPadding = 20;
constexpr int kLaneSpacing = 28;
constexpr int kRowHeight = 72;
constexpr int kNodeRadius = 6;
constexpr int kTextStartPadding = 28;
constexpr int kRightPadding = 28;
constexpr int kBadgeHeight = 22;
constexpr int kBadgeHorizontalPadding = 8;
constexpr int kBadgeSpacing = 8;
constexpr int kBodyOffsetY = 18;
constexpr int kMetaOffsetY = 40;
constexpr int kSummaryOffsetY = 10;
constexpr int kLineThickness = 3;

wxString toWxString(const std::string& value) {
    return wxString::FromUTF8(value);
}

wxColour laneColor(int lane) {
    static const wxColour kPalette[] = {
        wxColour(50, 122, 255),
        wxColour(255, 159, 10),
        wxColour(52, 199, 89),
        wxColour(191, 90, 242),
        wxColour(255, 69, 58)
    };

    return kPalette[lane % (sizeof(kPalette) / sizeof(kPalette[0]))];
}

wxColour badgeFillColor(const std::string& colorKey) {
    if (colorKey == "accent") {
        return wxColour(17, 94, 89);
    }
    if (colorKey == "primary") {
        return wxColour(30, 64, 175);
    }
    if (colorKey == "neutral") {
        return wxColour(55, 65, 81);
    }
    if (colorKey == "warning") {
        return wxColour(146, 64, 14);
    }
    return wxColour(71, 85, 105);
}

wxString metaLineForCommit(const domain::CommitNode& commit) {
    wxString meta = toWxString(commit.authorName);
    meta += "  ";
    meta += toWxString(commit.authorTime);
    if (commit.isMergeCommit) {
        meta += "  merge";
    }
    return meta;
}

int maxLaneIndex(const std::vector<domain::CommitNode>& graph) {
    int maxLane = 0;
    for (const auto& commit : graph) {
        maxLane = std::max(maxLane, commit.graphLane);
    }
    return maxLane;
}

int laneCenterX(int lane) {
    return kLeftPadding + lane * kLaneSpacing;
}

int rowCenterY(int row) {
    return kTopPadding + row * kRowHeight + (kRowHeight / 2);
}

}  // namespace

CommitGraphView::CommitGraphView(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                       wxBORDER_NONE | wxVSCROLL | wxHSCROLL) {
    SetScrollRate(12, 12);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &CommitGraphView::paintEvent, this);
    Bind(wxEVT_SIZE, &CommitGraphView::sizeEvent, this);
}

void CommitGraphView::setGraph(std::vector<domain::CommitNode> graph) {
    graph_ = std::move(graph);
    updateVirtualSize();
    Refresh();
}

void CommitGraphView::updateVirtualSize() {
    const int minWidth = std::max(GetClientSize().GetWidth(), graphWidth());
    SetVirtualSize(minWidth, totalHeight());
}

void CommitGraphView::paintEvent(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);

    const wxSize clientSize = GetClientSize();
    dc.SetBackground(wxBrush(wxColour(248, 250, 252)));
    dc.Clear();

    if (graph_.empty()) {
        dc.SetTextForeground(wxColour(100, 116, 139));
        dc.DrawText("Commit graph will appear after loading repository history.", 24, 24);
        return;
    }

    std::unordered_map<std::string, std::size_t> commitIndexByOid;
    for (std::size_t index = 0; index < graph_.size(); ++index) {
        commitIndexByOid.emplace(graph_[index].oid, index);
    }

    dc.SetFont(wxFontInfo(10));
    for (std::size_t row = 0; row < graph_.size(); ++row) {
        const auto& commit = graph_[row];
        const int currentX = laneCenterX(commit.graphLane);
        const int currentY = rowCenterY(static_cast<int>(row));

        for (const auto& parentOid : commit.parents) {
            const auto parentIt = commitIndexByOid.find(parentOid);
            if (parentIt == commitIndexByOid.end()) {
                continue;
            }

            const auto& parent = graph_[parentIt->second];
            const int parentX = laneCenterX(parent.graphLane);
            const int parentY = rowCenterY(static_cast<int>(parentIt->second));
            const int elbowY = currentY + ((parentY - currentY) / 2);

            dc.SetPen(wxPen(laneColor(std::max(commit.graphLane, parent.graphLane)), kLineThickness));
            dc.DrawLine(currentX, currentY, currentX, elbowY);
            dc.DrawLine(currentX, elbowY, parentX, elbowY);
            dc.DrawLine(parentX, elbowY, parentX, parentY);
        }
    }

    const int detailsStartX = laneCenterX(maxLaneIndex(graph_) + 1) + kTextStartPadding;
    const int detailsWidth = std::max(360, clientSize.GetWidth() - detailsStartX - kRightPadding);

    for (std::size_t row = 0; row < graph_.size(); ++row) {
        const auto& commit = graph_[row];
        const int nodeX = laneCenterX(commit.graphLane);
        const int nodeY = rowCenterY(static_cast<int>(row));
        const int rowTop = nodeY - (kRowHeight / 2) + 6;
        const wxRect rowRect(detailsStartX - 14, rowTop, detailsWidth, kRowHeight - 12);

        dc.SetPen(wxPen(wxColour(226, 232, 240), 1));
        dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
        dc.DrawRoundedRectangle(rowRect, 10);

        dc.SetPen(wxPen(wxColour(255, 255, 255), 2));
        dc.SetBrush(wxBrush(laneColor(commit.graphLane)));
        dc.DrawCircle(wxPoint(nodeX, nodeY), kNodeRadius + (commit.isMergeCommit ? 1 : 0));

        if (commit.isMergeCommit) {
            dc.SetPen(wxPen(wxColour(255, 255, 255), 2));
            dc.DrawLine(nodeX - 4, nodeY, nodeX + 4, nodeY);
            dc.DrawLine(nodeX, nodeY - 4, nodeX, nodeY + 4);
        }

        const int textX = rowRect.GetLeft() + 16;
        const int summaryY = rowTop + kSummaryOffsetY;
        const int metaY = rowTop + kMetaOffsetY;

        dc.SetFont(wxFontInfo(11).Bold());
        dc.SetTextForeground(wxColour(15, 23, 42));
        dc.DrawText(toWxString(commit.summary), textX, summaryY);

        wxSize summarySize = dc.GetTextExtent(toWxString(commit.summary));
        int badgeX = textX + summarySize.GetWidth() + 14;

        dc.SetFont(wxFontInfo(9).Bold());
        for (const auto& ref : commit.refs) {
            const wxString label = toWxString(ref.name);
            const wxSize textSize = dc.GetTextExtent(label);
            const int badgeWidth = textSize.GetWidth() + (kBadgeHorizontalPadding * 2);
            const wxRect badgeRect(badgeX, summaryY - 2, badgeWidth, kBadgeHeight);

            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(badgeFillColor(ref.colorKey)));
            dc.DrawRoundedRectangle(badgeRect, 11);
            dc.SetTextForeground(wxColour(248, 250, 252));
            dc.DrawText(label, badgeX + kBadgeHorizontalPadding, summaryY + 1);

            badgeX += badgeWidth + kBadgeSpacing;
        }

        dc.SetFont(wxFontInfo(9));
        dc.SetTextForeground(wxColour(71, 85, 105));
        dc.DrawText(toWxString(commit.shortOid), textX, metaY);

        const wxSize oidSize = dc.GetTextExtent(toWxString(commit.shortOid));
        dc.DrawText(metaLineForCommit(commit), textX + oidSize.GetWidth() + 14, metaY);

        if (!commit.parents.empty()) {
            wxString parentLabel = "Parents: ";
            for (std::size_t parentIndex = 0; parentIndex < commit.parents.size(); ++parentIndex) {
                if (parentIndex > 0) {
                    parentLabel += ", ";
                }
                parentLabel += toWxString(commit.parents[parentIndex].substr(0, 7));
            }

            dc.SetTextForeground(wxColour(100, 116, 139));
            dc.DrawText(parentLabel, textX, rowTop + kBodyOffsetY);
        }
    }
}

void CommitGraphView::sizeEvent(wxSizeEvent& event) {
    updateVirtualSize();
    event.Skip();
}

int CommitGraphView::graphWidth() const {
    const int laneCount = maxLaneIndex(graph_) + 1;
    const int detailsWidth = 620;
    return kLeftPadding + laneCount * kLaneSpacing + kTextStartPadding + detailsWidth + kRightPadding;
}

int CommitGraphView::totalHeight() const {
    const int graphHeight = static_cast<int>(graph_.size()) * kRowHeight;
    return kTopPadding + graphHeight + kBottomPadding;
}

}  // namespace ogc::ui
