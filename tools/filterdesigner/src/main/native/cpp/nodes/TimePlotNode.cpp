// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/TimePlotNode.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/JsonInt.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/nodes/PlotPalette.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <cstdio>
#include <limits>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/graph/Topology.hpp"
#endif

namespace wpi::filterdesigner {

namespace {
constexpr std::array<const char*, TimePlotNode::kInputCount> kInputNames = {
    "in0", "in1", "in2", "in3"};
}  // namespace

TimePlotNode::TimePlotNode() : m_logic(std::make_unique<TimePlotNodeLogic>()) {
  setTitle("Time Plot");
  setStyle(ImFlow::NodeStyle::cyan());
  for (int i = 0; i < kInputCount; ++i) {
    // Link::update reads the destination pin's style, and the series is
    // forced to match below, so wire, socket and curve agree per slot
    // whatever order the inputs were wired in.
    auto style = std::make_shared<ImFlow::PinStyle>(PlotPaletteU32(i), 0, 4.f,
                                                    4.67f, 3.7f, 1.f);
    addIN<const wpi::filterdesigner::Signal*>(
        kInputNames[i], nullptr, ImFlow::ConnectionFilter::SameType(),
        std::move(style));
  }
}

TimePlotNode::~TimePlotNode() = default;

void TimePlotNode::SerializeParams(wpi::util::json& obj) const {
  obj["autoscale"] = m_logic->autoscale;
  obj["showLegend"] = m_logic->showLegend;
  // Clamp on the way out too, so a hand-edited file can't ping-pong an
  // out-of-range value across save/load cycles.
  obj["plotWidth"] =
      std::max(TimePlotNodeLogic::kMinPlotWidth, m_logic->plotWidth);
  obj["plotHeight"] =
      std::max(TimePlotNodeLogic::kMinPlotHeight, m_logic->plotHeight);
}

void TimePlotNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("autoscale"); p && p->is_bool()) {
    m_logic->autoscale = p->get_bool();
  }
  if (const auto* p = obj.lookup("showLegend"); p && p->is_bool()) {
    m_logic->showLegend = p->get_bool();
  }
  if (const std::optional<float> v = ReadFloatField(obj, "plotWidth")) {
    m_logic->plotWidth = std::max(TimePlotNodeLogic::kMinPlotWidth, *v);
  }
  if (const std::optional<float> v = ReadFloatField(obj, "plotHeight")) {
    m_logic->plotHeight = std::max(TimePlotNodeLogic::kMinPlotHeight, *v);
  }
}

void TimePlotNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "TimePlot";
  entry.menuLabel = "Time Plot";
  entry.menuCategory = "Plots";
  entry.inputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<TimePlotNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void TimePlotNode::draw() {
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  ImGui::Checkbox("Autoscale", &m_logic->autoscale);
  ImGui::SameLine();
  ImGui::Checkbox("Legend", &m_logic->showLegend);

  std::array<const Signal*, kInputCount> signals{};
  int connected = 0;
  bool anyLive = false;
  for (int i = 0; i < kInputCount; ++i) {
    signals[i] = getInVal<const Signal*>(kInputNames[i]);
    if (signals[i]) {
      ++connected;
      if (signals[i]->live) {
        anyLive = true;
      }
    }
  }

  if (connected == 0) {
    ImGui::TextDisabled("Connect a Signal to in0..in3.");
    return;
  }

  ImVec2 plotSize{m_logic->plotWidth, m_logic->plotHeight};
  // ImPlot shows a legend by default; SetupLegend only positions it.
  ImPlotFlags plotFlags =
      m_logic->showLegend ? ImPlotFlags_None : ImPlotFlags_NoLegend;
  if (ImPlot::BeginPlot("##timeplot", plotSize, plotFlags)) {
    // A live source's timestamps drift upward forever, so AutoFit pins the
    // view to the buffered span. Static sources stay manual, keeping pan and
    // zoom.
    ImPlotAxisFlags xFlags =
        anyLive ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
    ImPlotAxisFlags yFlags =
        m_logic->autoscale ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
    ImPlot::SetupAxis(ImAxis_X1, "Time (s)", xFlags);
    ImPlot::SetupAxis(ImAxis_Y1, "Value", yFlags);
    if (m_logic->showLegend) {
      ImPlot::SetupLegend(ImPlotLocation_NorthEast);
    }
    for (int i = 0; i < kInputCount; ++i) {
      const Signal* sig = signals[i];
      if (!sig || sig->values.empty()) {
        continue;
      }
      int count = static_cast<int>(std::min<size_t>(
          sig->values.size(), std::numeric_limits<int>::max()));
      // The signal's own name, which distinguishes two logs' entries. The
      // hidden pin suffix keeps ImPlot's item id unique when two inputs
      // share a name — two Impulse nodes, say — so each curve gets its own
      // legend entry and visibility toggle.
      const std::string label =
          (sig->name.empty() ? std::string{kInputNames[i]} : sig->name) + "##" +
          kInputNames[i];
      ImPlot::PlotLine(label.c_str(), sig->timestamps.data(),
                       sig->values.data(), count,
                       {ImPlotProp_LineColor, PlotPaletteVec4(i)});
    }
    ImPlot::EndPlot();
  }

  // Safe as an interactive widget: ImNodeFlow only takes node-drag from the
  // header rectangle.
  const float kGripSize = 12.0f;
  ImVec2 plotBR = ImGui::GetItemRectMax();
  ImGui::SetCursorScreenPos(ImVec2{plotBR.x - kGripSize, plotBR.y - kGripSize});
  ImGui::InvisibleButton("##resize", ImVec2{kGripSize, kGripSize});
  bool hovered = ImGui::IsItemHovered();
  if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    ImVec2 delta = ImGui::GetIO().MouseDelta;
    m_logic->plotWidth = std::max(TimePlotNodeLogic::kMinPlotWidth,
                                  m_logic->plotWidth + delta.x);
    m_logic->plotHeight = std::max(TimePlotNodeLogic::kMinPlotHeight,
                                   m_logic->plotHeight + delta.y);
  }
  if (hovered || ImGui::IsItemActive()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
  }
  // Three short diagonal strokes, as ImGui's own window resize grips.
  ImU32 gripColor =
      ImGui::GetColorU32(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
  ImDrawList* dl = ImGui::GetWindowDrawList();
  for (int i = 0; i < 3; ++i) {
    float off = 2.0f + i * 3.5f;
    dl->AddLine(ImVec2{plotBR.x - off, plotBR.y - 2.0f},
                ImVec2{plotBR.x - 2.0f, plotBR.y - off}, gripColor, 1.5f);
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void TimePlotNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
