// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/TimePlotView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <algorithm>
#include <limits>

#include <imgui.h>
#include <implot.h>

namespace wpi::filterdesigner {

void TimePlotView::Display(const Signal* signal,
                           std::span<const double> filtered) {
  if (!signal) {
    ImGui::TextDisabled("Pick a numeric entry from Data Source.");
    return;
  }

  ImGui::Text("Signal: %s", signal->name.c_str());
  ImGui::SameLine();
  ImGui::TextDisabled("(%zu samples)", signal->values.size());
  ImGui::Text("Inferred sample rate: %.2f Hz  |  Uniform: %s",
              signal->sampleRate, signal->uniform ? "yes" : "no");

  if (signal->values.empty()) {
    ImGui::TextDisabled("Empty signal.");
    return;
  }

  if (ImPlot::BeginPlot("##timeplot", ImVec2{-1, -1})) {
    ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
    ImPlot::SetupAxis(ImAxis_Y1, "Value");
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);
    int count = static_cast<int>(std::min<size_t>(
        signal->values.size(), std::numeric_limits<int>::max()));
    ImPlot::PlotLine("Raw", signal->timestamps.data(), signal->values.data(),
                     count);
    if (!filtered.empty() && filtered.size() == signal->values.size()) {
      ImPlot::PlotLine("Filtered", signal->timestamps.data(), filtered.data(),
                       count);
    }
    ImPlot::EndPlot();
  }
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
