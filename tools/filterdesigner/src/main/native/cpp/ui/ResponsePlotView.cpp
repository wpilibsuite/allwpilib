// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/ResponsePlotView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <algorithm>
#include <limits>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/model/FilterResponse.hpp"

namespace wpi::filterdesigner {

void ResponsePlotView::Display(const std::optional<Sections>& sections,
                               double fs) {
  if (!sections.has_value() || sections->empty()) {
    ImGui::TextDisabled("No filter to plot.");
    return;
  }
  if (fs <= 0.0) {
    ImGui::TextDisabled("Sample rate must be positive.");
    return;
  }

  auto response = FrequencyResponse::Compute(*sections, fs);
  if (!response) {
    ImGui::TextDisabled("Response could not be computed.");
    return;
  }

  int count = static_cast<int>(std::min<size_t>(
      response->frequencies.size(), std::numeric_limits<int>::max()));

  if (ImPlot::BeginSubplots("##response", 2, 1, ImVec2{-1, -1},
                            ImPlotSubplotFlags_LinkAllX)) {
    if (ImPlot::BeginPlot("##magnitude")) {
      ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)", ImPlotAxisFlags_NoLabel);
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
      ImPlot::SetupAxis(ImAxis_Y1, "Magnitude (dB)");
      ImPlot::PlotLine("|H(f)|", response->frequencies.data(),
                       response->magnitudesDb.data(), count);
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##phase")) {
      ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
      ImPlot::SetupAxis(ImAxis_Y1, "Phase (deg)");
      ImPlot::PlotLine("∠H(f)", response->frequencies.data(),
                       response->phasesDegrees.data(), count);
      ImPlot::EndPlot();
    }
    ImPlot::EndSubplots();
  }
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
