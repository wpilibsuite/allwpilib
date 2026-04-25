// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/PoleZeroView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <cmath>
#include <numbers>
#include <vector>

#include <imgui.h>
#include <implot.h>

#include "wpi/filterdesigner/model/PoleZero.hpp"

namespace wpi::filterdesigner {

namespace {

constexpr int kUnitCircleSegments = 128;

void BuildUnitCircle(std::vector<double>& xs, std::vector<double>& ys) {
  xs.resize(kUnitCircleSegments + 1);
  ys.resize(kUnitCircleSegments + 1);
  for (int i = 0; i <= kUnitCircleSegments; ++i) {
    double theta = 2.0 * std::numbers::pi * static_cast<double>(i) /
                   static_cast<double>(kUnitCircleSegments);
    xs[i] = std::cos(theta);
    ys[i] = std::sin(theta);
  }
}

}  // namespace

void PoleZeroView::Display(const std::optional<Sections>& sections) {
  if (!sections.has_value() || sections->empty()) {
    ImGui::TextDisabled("No filter to plot.");
    return;
  }

  auto pz = ComputePolesZeros(*sections);

  static std::vector<double> circleX;
  static std::vector<double> circleY;
  if (circleX.empty()) {
    BuildUnitCircle(circleX, circleY);
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

  ImGui::TextDisabled("%zu pole%s, %zu zero%s", pz.poles.size(),
                      pz.poles.size() == 1 ? "" : "s", pz.zeros.size(),
                      pz.zeros.size() == 1 ? "" : "s");

  if (ImPlot::BeginPlot("##polezero", ImVec2{-1, -1}, ImPlotFlags_Equal)) {
    ImPlot::SetupAxis(ImAxis_X1, "Re");
    ImPlot::SetupAxis(ImAxis_Y1, "Im");
    ImPlot::SetupAxisLimits(ImAxis_X1, -1.5, 1.5, ImPlotCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, -1.5, 1.5, ImPlotCond_Once);

    ImPlot::PlotLine("unit circle", circleX.data(), circleY.data(),
                     static_cast<int>(circleX.size()));

    if (!zeroX.empty()) {
      ImPlot::PlotScatter(
          "zeros", zeroX.data(), zeroY.data(), static_cast<int>(zeroX.size()),
          {ImPlotProp_Marker, ImPlotMarker_Circle, ImPlotProp_MarkerSize, 6.0f,
           ImPlotProp_LineWeight, 1.5f});
    }
    if (!poleX.empty()) {
      ImPlot::PlotScatter(
          "poles", poleX.data(), poleY.data(), static_cast<int>(poleX.size()),
          {ImPlotProp_Marker, ImPlotMarker_Cross, ImPlotProp_MarkerSize, 6.0f,
           ImPlotProp_LineWeight, 1.5f});
    }
    ImPlot::EndPlot();
  }
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
