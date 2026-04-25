// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/TimeResponseView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <algorithm>
#include <vector>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/model/ApplyFilter.hpp"
#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"

namespace wpi::filterdesigner {

namespace {
constexpr const char* kLengthKey = "timeresp.length";
constexpr int kMinLength = 16;
constexpr int kMaxLength = 4096;
constexpr int kDefaultLength = 200;
}  // namespace

void TimeResponseView::Display(const std::optional<Sections>& sections) {
  auto& storage = wpi::glass::GetStorage();
  int& length = storage.GetInt(kLengthKey, kDefaultLength);
  length = std::clamp(length, kMinLength, kMaxLength);

  ImGui::SetNextItemWidth(120.0f);
  ImGui::InputInt("Length (samples)", &length);

  if (!sections.has_value() || sections->empty()) {
    ImGui::TextDisabled("No filter to plot.");
    return;
  }

  const size_t n = static_cast<size_t>(length);

  std::vector<double> impulse(n, 0.0);
  impulse[0] = 1.0;
  std::vector<double> step(n, 1.0);

  auto impulseOut = ApplyFilter(impulse, *sections);
  auto stepOut = ApplyFilter(step, *sections);

  std::vector<double> indices(n);
  for (size_t i = 0; i < n; ++i) {
    indices[i] = static_cast<double>(i);
  }

  if (ImPlot::BeginSubplots("##timeresp", 2, 1, ImVec2{-1, -1},
                            ImPlotSubplotFlags_LinkAllX)) {
    if (ImPlot::BeginPlot("##impulse")) {
      ImPlot::SetupAxis(ImAxis_X1, "Sample", ImPlotAxisFlags_NoLabel);
      ImPlot::SetupAxis(ImAxis_Y1, "Impulse h[n]");
      ImPlot::PlotLine("h[n]", indices.data(), impulseOut.data(),
                       static_cast<int>(n));
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##step")) {
      ImPlot::SetupAxis(ImAxis_X1, "Sample");
      ImPlot::SetupAxis(ImAxis_Y1, "Step y[n]");
      ImPlot::PlotLine("y[n]", indices.data(), stepOut.data(),
                       static_cast<int>(n));
      ImPlot::EndPlot();
    }
    ImPlot::EndSubplots();
  }
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
