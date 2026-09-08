// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/WpiLogSourceNode.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/gui/portable-file-dialogs.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <format>
#include <span>
#include <string_view>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/nodes/NameTreePicker.hpp"
#include "wpi/filterdesigner/nodes/SamplingReadout.hpp"
#include "wpi/filterdesigner/nodes/StatusText.hpp"
#endif

namespace wpi::filterdesigner {

WpiLogSourceNode::WpiLogSourceNode()
    : m_logic(std::make_unique<WpiLogSourceNodeLogic>()) {
  setTitle("WPILOG Source");
  setStyle(ImFlow::NodeStyle::green());
  // The unique_ptr keeps the logic alive for the node's lifetime, and
  // ImNodeFlow tears links down before the OutPin, so this can't dangle.
  auto* logic = m_logic.get();
  addOUT<const wpi::filterdesigner::Signal*>("out")->behaviour(
      [logic] { return logic->Signal(); });
}

WpiLogSourceNode::~WpiLogSourceNode() = default;

void WpiLogSourceNode::SerializeParams(wpi::util::json& obj) const {
  obj["logPath"] = m_logic->LogPath();
  obj["entry"] = m_logic->SelectedEntry();
  // Only alongside an entry: a window is a pair of timestamps into one
  // entry's record and means nothing without it.
  if (!m_logic->SelectedEntry().empty()) {
    obj["rangeStart"] = m_logic->SelectedRange().start;
    obj["rangeEnd"] = m_logic->SelectedRange().end;
  }
}

void WpiLogSourceNode::DeserializeParams(const wpi::util::json& obj) {
  std::string path;
  std::string entry;
  if (const auto* p = obj.lookup("logPath"); p && p->is_string()) {
    path = p->get_string();
  }
  if (const auto* e = obj.lookup("entry"); e && e->is_string()) {
    entry = e->get_string();
  }
  m_entrySearch.clear();
  m_logic->RestoreFromPath(path, entry);

  // After the entry, which resets the window to the whole record. A range
  // that no longer fits clamps, and one that clamps to nothing is refused,
  // leaving the whole record selected.
  const auto* start = obj.lookup("rangeStart");
  const auto* end = obj.lookup("rangeEnd");
  if (start && start->is_number() && end && end->is_number()) {
    m_logic->SelectRange(TimeRange{start->get_number(), end->get_number()});
  }
}

void WpiLogSourceNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "WpiLogSource";
  entry.menuLabel = "WPILOG Source";
  entry.menuCategory = "Sources";
  entry.outputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<WpiLogSourceNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

namespace {

// The entry picker and the timeline strip both stretch to the node's width;
// this is only the floor, for a node whose other rows are all short. The strip
// is an orientation and selection surface, not the analysis plot — that is
// what wiring a Time Plot to the out pin is for.
constexpr float kMinContentWidth = 240.0f;
constexpr float kTimelineHeight = 110.0f;
// Overview points drawn per frame. A 600 s entry at 250 Hz is 150k samples
// across a few hundred pixels, so striding costs nothing anyone can see.
constexpr std::size_t kTimelinePoints = 2000;
// A marker released within this fraction of the visible span of a segment edge
// lands on it exactly. A fraction rather than a pixel count, so the gesture is
// the same at every zoom level.
constexpr double kSnapFraction = 0.02;
// Shortest drag, in pixels, that counts as drawing a window rather than as a
// stray Shift+click.
constexpr float kMinWindowPixels = 4.0f;

// The pauses, and everything outside the window, are darkened rather than
// hidden: what a selection leaves out is the point of looking at the strip.
constexpr ImU32 kGapFill = IM_COL32(0, 0, 0, 90);
constexpr ImU32 kOutsideFill = IM_COL32(0, 0, 0, 110);

/** Nearest segment edge to @p t within @p tolerance, else @p t unchanged. */
double SnapToSegmentEdge(double t, std::span<const Segment> segments,
                         double tolerance) {
  double best = t;
  double bestDistance = tolerance;
  for (const auto& segment : segments) {
    for (const double edge : {segment.start, segment.end}) {
      const double distance = std::abs(edge - t);
      if (distance < bestDistance) {
        bestDistance = distance;
        best = edge;
      }
    }
  }
  return best;
}

}  // namespace

void WpiLogSourceNode::DrawTimeline() {
  const auto* raw = m_logic->RawSignal();
  const TimeRange full = m_logic->FullRange();
  // An entry spanning no time — one sample, or several sharing a timestamp —
  // has no axis to draw and nothing to select within.
  if (!raw || raw->timestamps.size() < 2 || full.Duration() <= 0.0) {
    return;
  }
  const std::span<const Segment> segments = m_logic->Segments();

  if (ImGui::SmallButton("Full record")) {
    m_logic->SelectFullRange();
  }
  ImGui::SameLine();
  ImGui::BeginDisabled(segments.empty());
  if (ImGui::SmallButton("Longest segment")) {
    m_logic->SelectLongestSegment();
  }
  ImGui::EndDisabled();

  // A drag that committed every frame would re-slice and resample the entry
  // each time, invalidating every downstream cache. The markers ride a pending
  // copy between mouse-down and release, so the window changes exactly once.
  if (!m_dragging) {
    m_pending = m_logic->SelectedRange();
  }
  const double low = std::min(m_pending.start, m_pending.end);
  const double high = std::max(m_pending.start, m_pending.end);

  // Read off the markers rather than the committed window, so the numbers
  // move with the drag even though the output pin does not.
  std::string summary =
      std::format("{:.2f} to {:.2f} s of {:.2f} s", low, high, full.Duration());
  // The segment count is why the strip looks striped, and how far there is
  // still to narrow.
  if (segments.size() > 1) {
    summary += std::format(", {} segments", segments.size());
  }
  ImGui::TextDisabled("%s", summary.c_str());

  // Shift+drag draws the window on the same button ImPlot pans with, so for
  // the frames Shift is down Pan is parked on a button nobody is pressing —
  // held through m_selecting too, or releasing Shift mid-drag would pan. The
  // map is global state, so it goes back on the way out.
  ImPlotInputMap& inputMap = ImPlot::GetInputMap();
  const ImGuiMouseButton savedPan = inputMap.Pan;
  if (ImGui::GetIO().KeyShift || m_selecting) {
    inputMap.Pan = ImGuiMouseButton_Middle;
  }

  if (!ImPlot::BeginPlot(
          "##timeline",
          ImVec2{std::max(kMinContentWidth, m_contentWidth), kTimelineHeight},
          ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus |
              ImPlotFlags_NoMouseText)) {
    inputMap.Pan = savedPan;
    return;
  }
  ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoLabel);
  ImPlot::SetupAxis(ImAxis_Y1, nullptr,
                    ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit);
  // Fit to the record the first time and again whenever the record changes,
  // but not in between: the view is where the user panned and zoomed to.
  ImPlot::SetupAxisLimits(
      ImAxis_X1, full.start, full.end,
      m_fittedSpan == full ? ImPlotCond_Once : ImPlotCond_Always);
  ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, full.start, full.end);
  m_fittedSpan = full;

  ImDrawList* draw = ImPlot::GetPlotDrawList();
  const ImVec2 plotPos = ImPlot::GetPlotPos();
  const ImVec2 plotSize = ImPlot::GetPlotSize();
  const float top = plotPos.y;
  const float bottom = plotPos.y + plotSize.y;

  // Shade the pauses: the holes ResampleToGrid has to invent values across,
  // and so the reason to narrow the window at all.
  ImPlot::PushPlotClipRect();
  for (std::size_t i = 1; i < segments.size(); ++i) {
    const float x0 =
        ImPlot::PlotToPixels(ImPlotPoint{segments[i - 1].end, 0.0}).x;
    const float x1 =
        ImPlot::PlotToPixels(ImPlotPoint{segments[i].start, 0.0}).x;
    // Entries with thousands of segments are common, and at this width most
    // of their gaps land inside one pixel; drawing them would be a grey wash.
    if (x1 - x0 >= 1.0f) {
      draw->AddRectFilled(ImVec2{x0, top}, ImVec2{x1, bottom}, kGapFill);
    }
  }
  ImPlot::PopPlotClipRect();

  const std::size_t count = raw->timestamps.size();
  const std::size_t step =
      std::max<std::size_t>(1, (count + kTimelinePoints - 1) / kTimelinePoints);
  ImPlot::PlotLine(
      "##raw", raw->timestamps.data(), raw->values.data(),
      static_cast<int>((count + step - 1) / step),
      ImPlotSpec{ImPlotProp_Stride, static_cast<int>(step * sizeof(double))});

  // Dim what the window leaves out, over the trace rather than under it, so
  // excluded data reads as excluded.
  const ImPlotRect limits = ImPlot::GetPlotLimits();
  ImPlot::PushPlotClipRect();
  if (low > limits.X.Min) {
    draw->AddRectFilled(
        ImVec2{plotPos.x, top},
        ImVec2{ImPlot::PlotToPixels(ImPlotPoint{low, 0.0}).x, bottom},
        kOutsideFill);
  }
  if (high < limits.X.Max) {
    draw->AddRectFilled(
        ImVec2{ImPlot::PlotToPixels(ImPlotPoint{high, 0.0}).x, top},
        ImVec2{plotPos.x + plotSize.x, bottom}, kOutsideFill);
  }
  ImPlot::PopPlotClipRect();

  const ImVec4 markerColor{1.0f, 0.78f, 0.25f, 1.0f};
  bool startHeld = false;
  bool endHeld = false;
  ImPlot::DragLineX(0, &m_pending.start, markerColor, 2.0f,
                    ImPlotDragToolFlags_NoFit, nullptr, nullptr, &startHeld);
  ImPlot::DragLineX(1, &m_pending.end, markerColor, 2.0f,
                    ImPlotDragToolFlags_NoFit, nullptr, nullptr, &endHeld);

  // Press is one edge, release the other, so a single gesture names both.
  // A click carries one position and no way to say which edge it meant.
  if (!startHeld && !endHeld && !m_selecting && ImPlot::IsPlotHovered() &&
      ImGui::GetIO().KeyShift && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    m_selecting = true;
    m_selectAnchor = ImPlot::GetPlotMousePos().x;
  }
  if (m_selecting) {
    const double cursor = ImPlot::GetPlotMousePos().x;
    m_pending.start = std::min(m_selectAnchor, cursor);
    m_pending.end = std::max(m_selectAnchor, cursor);
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
      m_selecting = false;
    }
  }

  if (startHeld || endHeld || m_selecting) {
    m_dragging = true;
  } else if (m_dragging) {
    m_dragging = false;
    const double committedLow = std::min(m_pending.start, m_pending.end);
    const double committedHigh = std::max(m_pending.start, m_pending.end);
    // A gesture that barely moved is a click, not a window. Publishing a few
    // pixels of record for a stray Shift+click would be worse than nothing.
    const double minimum =
        limits.X.Size() * (kMinWindowPixels / std::max(1.0f, plotSize.x));
    const double tolerance = limits.X.Size() * kSnapFraction;
    TimeRange committed{SnapToSegmentEdge(committedLow, segments, tolerance),
                        SnapToSegmentEdge(committedHigh, segments, tolerance)};
    if (committedHigh - committedLow < minimum ||
        !m_logic->SelectRange(committed)) {
      // Too small, or dragged off the record: put the markers back on what
      // is actually published.
      m_pending = m_logic->SelectedRange();
    }
  }

  ImPlot::EndPlot();
  inputMap.Pan = savedPan;

  // Shift+drag is not a gesture anyone guesses at, and the markers are the
  // only other way in.
  if (!m_dragging && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
    ImGui::SetTooltip(
        "Shift+drag: set the window\n"
        "Drag a marker: move one edge\n"
        "Drag: pan   Scroll: zoom   Double-click: fit");
  }
}

void WpiLogSourceNode::PollFileDialog() {
  if (!m_opener || !m_opener->ready(0)) {
    return;
  }
  auto result = m_opener->result();
  m_opener.reset();
  if (result.empty()) {
    return;
  }
  // A query that survived into a different log's entry list would hide every
  // row of it, which reads as a log that failed to open.
  m_entrySearch.clear();
  m_logic->OpenFile(result.front());
}

void WpiLogSourceNode::draw() {
  // Measure what the body draws so DrawTimeline can stretch to it next frame.
  // ImNodeFlow sizes a node after draw() returns, and GetContentRegionAvail()
  // inside a node reports the whole canvas.
  ImGui::BeginGroup();
  DrawBody();
  ImGui::EndGroup();
  m_contentWidth = ImGui::GetItemRectSize().x;
}

void WpiLogSourceNode::DrawBody() {
  PollFileDialog();

  if (ImGui::Button("Open .wpilog...")) {
    if (!m_opener) {
      m_opener = std::make_unique<pfd::open_file>(
          "Select Data Log", "",
          std::vector<std::string>{"DataLog Files", "*.wpilog"});
    }
  }
  if (!m_logic->LogPath().empty()) {
    ImGui::SameLine();
    ImGui::TextUnformatted(
        std::filesystem::path{m_logic->LogPath()}.filename().string().c_str());
  }
  if (!m_logic->LoadError().empty()) {
    DrawStatusText(kStatusErrorColor, m_logic->LoadError(),
                   std::max(kMinContentWidth, m_contentWidth));
  }
  if (!m_logic->HasFile()) {
    ImGui::TextDisabled("No log loaded.");
    return;
  }

  auto entries = m_logic->Entries();
  ImGui::TextDisabled("Entries (%zu):", entries.size());

  // Same width as the timeline, for the same reason: a log's entry names run
  // long and 220 px truncated most of them to "NT:/Spindexer/Vel...".
  ImGui::SetNextItemWidth(std::max(kMinContentWidth, m_contentWidth));
  const char* currentLabel = m_logic->SelectedEntry().empty()
                                 ? "<none>"
                                 : m_logic->SelectedEntry().c_str();
  if (ImGui::BeginCombo("##entry", currentLabel, ImGuiComboFlags_HeightLarge)) {
    DrawNameTreePicker(
        m_logic->EntryTree(), m_entrySearch, m_logic->SelectedEntry(),
        [this](std::string_view name) { m_logic->SelectEntry(name); });
    ImGui::EndCombo();
  }

  // Timeline above the readout, so the numbers sit directly under the window
  // that produced them.
  DrawTimeline();

  if (const auto* signal = m_logic->Signal()) {
    DrawSamplingReadout(*signal, std::max(kMinContentWidth, m_contentWidth));
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void WpiLogSourceNode::PollFileDialog() {}
void WpiLogSourceNode::DrawBody() {}
void WpiLogSourceNode::DrawTimeline() {}
void WpiLogSourceNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
