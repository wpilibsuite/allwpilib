// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <imgui.h>

namespace wpi::filterdesigner {

/**
 * Per-input-pin colors for plot-style sink nodes.
 *
 * The four-input plot sinks (Time, Bode, Frequency, Pole/Zero) share one color
 * contract: pin `inN` takes the Nth color, so does the destination end of its
 * wire (ImNodeFlow's Link::update), and so does the series it feeds. The wire
 * then traces the data into its curve, which ImPlot's auto-cycling cannot
 * promise — it depends on series-add order across the whole plot.
 *
 * The colors are the first four of ImPlot's "Deep" palette, near enough to the
 * auto-assigned ones to pass for them.
 */
inline constexpr std::array<ImU32, 4> kPlotPaletteU32 = {
    IM_COL32(76, 114, 176, 255),  // blue
    IM_COL32(221, 132, 82, 255),  // orange
    IM_COL32(85, 168, 104, 255),  // green
    IM_COL32(196, 78, 82, 255),   // red
};

inline ImVec4 PlotPaletteVec4(int index) {
  return ImGui::ColorConvertU32ToFloat4(
      kPlotPaletteU32[index % static_cast<int>(kPlotPaletteU32.size())]);
}

inline ImU32 PlotPaletteU32(int index) {
  return kPlotPaletteU32[index % static_cast<int>(kPlotPaletteU32.size())];
}

}  // namespace wpi::filterdesigner
