// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/PoleZeroPlotNode.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <cmath>
#include <cstdio>
#include <numbers>
#include <string>
#include <vector>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/graph/Topology.hpp"
#include "wpi/filterdesigner/model/PoleZero.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"
#endif

namespace wpi::filterdesigner {

namespace {
constexpr std::array<const char*, PoleZeroPlotNode::kInputCount> kInputNames = {
    "in0", "in1", "in2", "in3"};
}  // namespace

PoleZeroPlotNode::PoleZeroPlotNode()
    : m_logic(std::make_unique<PoleZeroPlotNodeLogic>()) {
  setTitle("Pole/Zero Plot");
  setStyle(ImFlow::NodeStyle::cyan());
  for (const char* name : kInputNames) {
    addIN<const wpi::filterdesigner::DesignedFilter*>(
        name, nullptr, ImFlow::ConnectionFilter::SameType());
  }
}

PoleZeroPlotNode::~PoleZeroPlotNode() = default;

void PoleZeroPlotNode::SerializeParams(wpi::util::json& obj) const {
  obj["showLegend"] = m_logic->showLegend;
  obj["plotWidth"] =
      std::max(PoleZeroPlotNodeLogic::kMinPlotWidth, m_logic->plotWidth);
  obj["plotHeight"] =
      std::max(PoleZeroPlotNodeLogic::kMinPlotHeight, m_logic->plotHeight);
}

void PoleZeroPlotNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("showLegend"); p && p->is_bool()) {
    m_logic->showLegend = p->get_bool();
  }
  if (const auto* p = obj.lookup("plotWidth"); p && p->is_number()) {
    m_logic->plotWidth = std::max(PoleZeroPlotNodeLogic::kMinPlotWidth,
                                  static_cast<float>(p->get_number()));
  }
  if (const auto* p = obj.lookup("plotHeight"); p && p->is_number()) {
    m_logic->plotHeight = std::max(PoleZeroPlotNodeLogic::kMinPlotHeight,
                                   static_cast<float>(p->get_number()));
  }
}

void PoleZeroPlotNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "PoleZeroPlot";
  entry.menuLabel = "Pole/Zero Plot";
  entry.menuCategory = "Plots";
  entry.inputTypes.emplace_back(
      typeid(const wpi::filterdesigner::DesignedFilter*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<PoleZeroPlotNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

namespace {

constexpr int kUnitCircleSegments = 128;

void BuildUnitCircle(std::vector<double>& xs, std::vector<double>& ys) {
  xs.resize(kUnitCircleSegments + 1);
  ys.resize(kUnitCircleSegments + 1);
  for (int i = 0; i <= kUnitCircleSegments; ++i) {
    double theta = 2.0 * std::numbers::pi * static_cast<double>(i) /
                   static_cast<double>(kUnitCircleSegments);
    xs[static_cast<std::size_t>(i)] = std::cos(theta);
    ys[static_cast<std::size_t>(i)] = std::sin(theta);
  }
}

}  // namespace

void PoleZeroPlotNode::draw() {
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  ImGui::Checkbox("Legend", &m_logic->showLegend);

  std::array<const DesignedFilter*, kInputCount> filters{};
  int connected = 0;
  for (int i = 0; i < kInputCount; ++i) {
    filters[i] = getInVal<const DesignedFilter*>(kInputNames[i]);
    if (filters[i]) {
      ++connected;
    }
  }

  // Surface upstream cascade errors for wired-but-errored inputs.
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

  // Precompute roots up-front so the plot loop is plain plotting.
  std::array<PoleZeroPlot, kInputCount> roots;
  for (int i = 0; i < kInputCount; ++i) {
    if (filters[i] && !filters[i]->sections.empty()) {
      roots[i] = ComputePolesZeros(filters[i]->sections);
    }
  }

  // Unit circle is the same every frame; build once and keep.
  static std::vector<double> circleX;
  static std::vector<double> circleY;
  if (circleX.empty()) {
    BuildUnitCircle(circleX, circleY);
  }

  ImVec2 plotSize{m_logic->plotWidth, m_logic->plotHeight};
  if (ImPlot::BeginPlot("##polezero", plotSize, ImPlotFlags_Equal)) {
    ImPlot::SetupAxis(ImAxis_X1, "Re");
    ImPlot::SetupAxis(ImAxis_Y1, "Im");
    ImPlot::SetupAxisLimits(ImAxis_X1, -1.5, 1.5, ImPlotCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, -1.5, 1.5, ImPlotCond_Once);
    if (m_logic->showLegend) {
      ImPlot::SetupLegend(ImPlotLocation_NorthEast);
    }

    ImPlot::PlotLine("unit circle", circleX.data(), circleY.data(),
                     static_cast<int>(circleX.size()));

    for (int i = 0; i < kInputCount; ++i) {
      const auto& pz = roots[i];
      if (pz.poles.empty() && pz.zeros.empty()) {
        continue;
      }
      std::vector<double> poleX;
      std::vector<double> poleY;
      std::vector<double> zeroX;
      std::vector<double> zeroY;
      poleX.reserve(pz.poles.size());
      poleY.reserve(pz.poles.size());
      zeroX.reserve(pz.zeros.size());
      zeroY.reserve(pz.zeros.size());
      for (const auto& p : pz.poles) {
        poleX.push_back(p.real());
        poleY.push_back(p.imag());
      }
      for (const auto& z : pz.zeros) {
        zeroX.push_back(z.real());
        zeroY.push_back(z.imag());
      }
      char labelZ[32];
      char labelP[32];
      std::snprintf(labelZ, sizeof(labelZ), "in%d zeros", i);
      std::snprintf(labelP, sizeof(labelP), "in%d poles", i);
      if (!zeroX.empty()) {
        ImPlot::PlotScatter(
            labelZ, zeroX.data(), zeroY.data(), static_cast<int>(zeroX.size()),
            {ImPlotProp_Marker, ImPlotMarker_Circle, ImPlotProp_MarkerSize,
             6.0f, ImPlotProp_LineWeight, 1.5f});
      }
      if (!poleX.empty()) {
        ImPlot::PlotScatter(
            labelP, poleX.data(), poleY.data(), static_cast<int>(poleX.size()),
            {ImPlotProp_Marker, ImPlotMarker_Cross, ImPlotProp_MarkerSize, 6.0f,
             ImPlotProp_LineWeight, 1.5f});
      }
    }
    ImPlot::EndPlot();
  }

  // Drag-resize grip — same affordance as the other plot nodes.
  const float kGripSize = 12.0f;
  ImVec2 plotBR = ImGui::GetItemRectMax();
  ImGui::SetCursorScreenPos(ImVec2{plotBR.x - kGripSize, plotBR.y - kGripSize});
  ImGui::InvisibleButton("##resize", ImVec2{kGripSize, kGripSize});
  bool hovered = ImGui::IsItemHovered();
  if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    ImVec2 delta = ImGui::GetIO().MouseDelta;
    m_logic->plotWidth = std::max(PoleZeroPlotNodeLogic::kMinPlotWidth,
                                  m_logic->plotWidth + delta.x);
    m_logic->plotHeight = std::max(PoleZeroPlotNodeLogic::kMinPlotHeight,
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

void PoleZeroPlotNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
