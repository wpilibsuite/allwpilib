// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/FrequencyPlotNode.hpp"

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
#include "wpi/filterdesigner/model/Spectrum.hpp"
#endif

namespace wpi::filterdesigner {

namespace {
constexpr std::array<const char*, FrequencyPlotNode::kInputCount> kInputNames =
    {"in0", "in1", "in2", "in3"};
}  // namespace

FrequencyPlotNode::FrequencyPlotNode()
    : m_logic(std::make_unique<FrequencyPlotNodeLogic>()) {
  setTitle("Frequency Plot");
  setStyle(ImFlow::NodeStyle::cyan());
  for (int i = 0; i < kInputCount; ++i) {
    auto style = std::make_shared<ImFlow::PinStyle>(PlotPaletteU32(i), 0, 4.f,
                                                    4.67f, 3.7f, 1.f);
    addIN<const wpi::filterdesigner::Signal*>(
        kInputNames[i], nullptr, ImFlow::ConnectionFilter::SameType(),
        std::move(style));
  }
}

FrequencyPlotNode::~FrequencyPlotNode() = default;

void FrequencyPlotNode::SerializeParams(wpi::util::json& obj) const {
  obj["autoscale"] = m_logic->autoscale;
  obj["showLegend"] = m_logic->showLegend;
  obj["logFrequency"] = m_logic->logFrequency;
  obj["plotWidth"] = m_logic->plotWidth;
  obj["plotHeight"] = m_logic->plotHeight;
}

void FrequencyPlotNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("autoscale"); p && p->is_bool()) {
    m_logic->autoscale = p->get_bool();
  }
  if (const auto* p = obj.lookup("showLegend"); p && p->is_bool()) {
    m_logic->showLegend = p->get_bool();
  }
  if (const auto* p = obj.lookup("logFrequency"); p && p->is_bool()) {
    m_logic->logFrequency = p->get_bool();
  }
  if (const std::optional<float> v = ReadFloatField(obj, "plotWidth")) {
    m_logic->plotWidth = std::max(FrequencyPlotNodeLogic::kMinPlotWidth, *v);
  }
  if (const std::optional<float> v = ReadFloatField(obj, "plotHeight")) {
    m_logic->plotHeight = std::max(FrequencyPlotNodeLogic::kMinPlotHeight, *v);
  }
}

void FrequencyPlotNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "FrequencyPlot";
  entry.menuLabel = "Frequency Plot";
  entry.menuCategory = "Plots";
  entry.inputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<FrequencyPlotNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void FrequencyPlotNode::draw() {
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  ImGui::Checkbox("Autoscale", &m_logic->autoscale);
  ImGui::SameLine();
  ImGui::Checkbox("Legend", &m_logic->showLegend);
  ImGui::SameLine();
  ImGui::Checkbox("Log x", &m_logic->logFrequency);

  std::array<const Signal*, kInputCount> signals{};
  int connected = 0;
  for (int i = 0; i < kInputCount; ++i) {
    signals[i] = getInVal<const Signal*>(kInputNames[i]);
    if (signals[i]) {
      ++connected;
    }
  }
  if (connected == 0) {
    ImGui::TextDisabled("Connect a Signal to in0..in3.");
    return;
  }

  // Fetch spectra up-front so the plot loop is plain plotting. Uniform
  // sampling is the loaders' job (see Signal::ResampleToGrid), so the inputs
  // dropped here are those with no usable rate, too few samples, or a
  // non-finite one. A signal whose gaps were too large to fill still gets an
  // FFT over nonuniform samples; its source node's readout is red.
  std::array<const Spectrum*, kInputCount> spectra{};
  for (int i = 0; i < kInputCount; ++i) {
    spectra[i] = m_logic->SpectrumFor(i, signals[i]);
  }

  // An absent curve otherwise reads as a pin with nothing wired to it.
  std::string gappy;
  for (int i = 0; i < kInputCount; ++i) {
    if (signals[i] && m_logic->HasGaps(i)) {
      if (!gappy.empty()) {
        gappy += ", ";
      }
      gappy += kInputNames[i];
    }
  }
  if (!gappy.empty()) {
    ImGui::TextDisabled("No spectrum for %s: non-finite samples.",
                        gappy.c_str());
  }

  ImVec2 plotSize{m_logic->plotWidth, m_logic->plotHeight};
  // ImPlot shows a legend by default; SetupLegend only positions it.
  ImPlotFlags plotFlags =
      m_logic->showLegend ? ImPlotFlags_None : ImPlotFlags_NoLegend;
  if (ImPlot::BeginPlot("##freqplot", plotSize, plotFlags)) {
    // Asked for only on the frame the scale changed (see TakeXAxisRefit),
    // and read here rather than beside the checkbox so a frame that never
    // reaches BeginPlot leaves the fit still owed.
    ImPlotAxisFlags xFlags = m_logic->TakeXAxisRefit() ? ImPlotAxisFlags_AutoFit
                                                       : ImPlotAxisFlags_None;
    ImPlotAxisFlags yFlags =
        m_logic->autoscale ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
    ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)", xFlags);
    if (m_logic->logFrequency) {
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
    }
    ImPlot::SetupAxis(ImAxis_Y1, "Magnitude (dB)", yFlags);
    if (m_logic->showLegend) {
      ImPlot::SetupLegend(ImPlotLocation_NorthEast);
    }
    // Bin zero is exactly 0 Hz, which a log axis clamps to DBL_MIN; fitting
    // to it spans hundreds of empty decades with every real bin against the
    // right edge. Skip it in log mode.
    const size_t firstBin = m_logic->logFrequency ? 1 : 0;
    for (int i = 0; i < kInputCount; ++i) {
      const Spectrum* spec = spectra[i];
      if (!spec || spec->frequencies.size() <= firstBin) {
        continue;
      }
      int count =
          static_cast<int>(std::min<size_t>(spec->frequencies.size() - firstBin,
                                            std::numeric_limits<int>::max()));
      // Hidden pin suffix: two inputs sharing a name would otherwise share
      // one ImPlot item, and so one legend entry and visibility toggle.
      const std::string label = ((signals[i] && !signals[i]->name.empty())
                                     ? signals[i]->name
                                     : std::string{kInputNames[i]}) +
                                "##" + kInputNames[i];
      ImPlot::PlotLine(label.c_str(), spec->frequencies.data() + firstBin,
                       spec->magnitudesDb.data() + firstBin, count,
                       {ImPlotProp_LineColor, PlotPaletteVec4(i)});
    }
    ImPlot::EndPlot();
  }

  const float kGripSize = 12.0f;
  ImVec2 plotBR = ImGui::GetItemRectMax();
  ImGui::SetCursorScreenPos(ImVec2{plotBR.x - kGripSize, plotBR.y - kGripSize});
  ImGui::InvisibleButton("##resize", ImVec2{kGripSize, kGripSize});
  bool hovered = ImGui::IsItemHovered();
  if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    ImVec2 delta = ImGui::GetIO().MouseDelta;
    m_logic->plotWidth = std::max(FrequencyPlotNodeLogic::kMinPlotWidth,
                                  m_logic->plotWidth + delta.x);
    m_logic->plotHeight = std::max(FrequencyPlotNodeLogic::kMinPlotHeight,
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

void FrequencyPlotNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
