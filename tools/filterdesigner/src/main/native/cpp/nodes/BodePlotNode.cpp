// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BodePlotNode.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <cstdio>
#include <limits>
#include <optional>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/graph/Topology.hpp"
#include "wpi/filterdesigner/model/FilterResponse.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#endif

namespace wpi::filterdesigner {

namespace {
constexpr std::array<const char*, BodePlotNode::kInputCount> kInputNames = {
    "in0", "in1", "in2", "in3"};
}  // namespace

BodePlotNode::BodePlotNode() : m_logic(std::make_unique<BodePlotNodeLogic>()) {
  setTitle("Bode Plot");
  setStyle(ImFlow::NodeStyle::cyan());
  for (const char* name : kInputNames) {
    addIN<const wpi::filterdesigner::DesignedFilter*>(
        name, nullptr, ImFlow::ConnectionFilter::SameType());
  }
}

BodePlotNode::~BodePlotNode() = default;

void BodePlotNode::SerializeParams(wpi::util::json& obj) const {
  obj["autoscale"] = m_logic->autoscale;
  obj["showLegend"] = m_logic->showLegend;
  obj["numPoints"] =
      std::clamp(m_logic->numPoints, BodePlotNodeLogic::kMinPoints,
                 BodePlotNodeLogic::kMaxPoints);
  obj["plotWidth"] =
      std::max(BodePlotNodeLogic::kMinPlotWidth, m_logic->plotWidth);
  obj["plotHeight"] =
      std::max(BodePlotNodeLogic::kMinPlotHeight, m_logic->plotHeight);
}

void BodePlotNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("autoscale"); p && p->is_bool()) {
    m_logic->autoscale = p->get_bool();
  }
  if (const auto* p = obj.lookup("showLegend"); p && p->is_bool()) {
    m_logic->showLegend = p->get_bool();
  }
  if (const auto* p = obj.lookup("numPoints"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    m_logic->numPoints = std::clamp(v, BodePlotNodeLogic::kMinPoints,
                                    BodePlotNodeLogic::kMaxPoints);
  }
  if (const auto* p = obj.lookup("plotWidth"); p && p->is_number()) {
    m_logic->plotWidth = std::max(BodePlotNodeLogic::kMinPlotWidth,
                                  static_cast<float>(p->get_number()));
  }
  if (const auto* p = obj.lookup("plotHeight"); p && p->is_number()) {
    m_logic->plotHeight = std::max(BodePlotNodeLogic::kMinPlotHeight,
                                   static_cast<float>(p->get_number()));
  }
}

void BodePlotNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "BodePlot";
  entry.menuLabel = "Bode Plot";
  entry.menuCategory = "Plots";
  entry.inputTypes.emplace_back(
      typeid(const wpi::filterdesigner::DesignedFilter*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<BodePlotNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void BodePlotNode::draw() {
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  ImGui::Checkbox("Autoscale", &m_logic->autoscale);
  ImGui::SameLine();
  ImGui::Checkbox("Legend", &m_logic->showLegend);

  std::array<const DesignedFilter*, kInputCount> filters{};
  int connected = 0;
  for (int i = 0; i < kInputCount; ++i) {
    filters[i] = getInVal<const DesignedFilter*>(kInputNames[i]);
    if (filters[i]) {
      ++connected;
    }
  }

  // Surface upstream cascade errors for pins that are wired but produced a
  // null filter — otherwise the user sees an empty plot with no hint that
  // the upstream BiquadStage is misconfigured.
  for (int i = 0; i < kInputCount; ++i) {
    if (filters[i]) {
      continue;
    }
    std::string upstreamErr =
        BiquadStageNode::UpstreamErrorFor(inPin(kInputNames[i]));
    if (!upstreamErr.empty()) {
      ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f}, "in%d: %s", i,
                         upstreamErr.c_str());
    }
  }

  if (connected == 0) {
    ImGui::TextDisabled("Connect a Filter to in0..in3.");
    return;
  }

  // Compute responses up-front so the subplot loop just plots; doing it
  // inside BeginPlot is fine too but the up-front pass keeps both subplots
  // in sync if one filter computes to no points (Compute returned nullopt).
  std::array<std::optional<FrequencyResponse>, kInputCount> responses;
  for (int i = 0; i < kInputCount; ++i) {
    if (filters[i] && !filters[i]->sections.empty() &&
        filters[i]->sampleRate > 0.0) {
      responses[i] = FrequencyResponse::Compute(
          filters[i]->sections, filters[i]->sampleRate, m_logic->numPoints);
    }
  }

  ImVec2 plotSize{m_logic->plotWidth, m_logic->plotHeight};
  if (ImPlot::BeginSubplots("##bode", 2, 1, plotSize,
                            ImPlotSubplotFlags_LinkAllX)) {
    ImPlotAxisFlags magFlags =
        m_logic->autoscale ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
    ImPlotAxisFlags phaseFlags = magFlags;
    if (ImPlot::BeginPlot("##magnitude")) {
      ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)", ImPlotAxisFlags_NoLabel);
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
      ImPlot::SetupAxis(ImAxis_Y1, "Magnitude (dB)", magFlags);
      if (m_logic->showLegend) {
        ImPlot::SetupLegend(ImPlotLocation_SouthWest);
      }
      for (int i = 0; i < kInputCount; ++i) {
        const auto& resp = responses[i];
        if (!resp) {
          continue;
        }
        int count = static_cast<int>(std::min<size_t>(
            resp->frequencies.size(), std::numeric_limits<int>::max()));
        char label[16];
        std::snprintf(label, sizeof(label), "in%d", i);
        ImPlot::PlotLine(label, resp->frequencies.data(),
                         resp->magnitudesDb.data(), count);
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##phase")) {
      ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
      ImPlot::SetupAxis(ImAxis_Y1, "Phase (deg)", phaseFlags);
      for (int i = 0; i < kInputCount; ++i) {
        const auto& resp = responses[i];
        if (!resp) {
          continue;
        }
        int count = static_cast<int>(std::min<size_t>(
            resp->frequencies.size(), std::numeric_limits<int>::max()));
        char label[16];
        std::snprintf(label, sizeof(label), "in%d", i);
        ImPlot::PlotLine(label, resp->frequencies.data(),
                         resp->phasesDegrees.data(), count);
      }
      ImPlot::EndPlot();
    }
    ImPlot::EndSubplots();
  }

  // Drag-resize grip in the bottom-right corner, same affordance as
  // TimePlotNode. Anchored to ImGui::GetItemRectMax() which points at the
  // end of the subplots block.
  const float kGripSize = 12.0f;
  ImVec2 plotBR = ImGui::GetItemRectMax();
  ImGui::SetCursorScreenPos(ImVec2{plotBR.x - kGripSize, plotBR.y - kGripSize});
  ImGui::InvisibleButton("##resize", ImVec2{kGripSize, kGripSize});
  bool hovered = ImGui::IsItemHovered();
  if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    ImVec2 delta = ImGui::GetIO().MouseDelta;
    m_logic->plotWidth = std::max(BodePlotNodeLogic::kMinPlotWidth,
                                  m_logic->plotWidth + delta.x);
    m_logic->plotHeight = std::max(BodePlotNodeLogic::kMinPlotHeight,
                                   m_logic->plotHeight + delta.y);
  }
  if (hovered || ImGui::IsItemActive()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
  }
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

void BodePlotNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
