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
 * The four-input plot sinks (Time, Bode, Frequency, Pole/Zero) all share the
 * same color contract: pin `inN` is the Nth color in this palette, the
 * destination-end wire is the same color (see ImNodeFlow's Link::update),
 * and the rendered series is forced to the same color via
 * `ImPlotProp_LineColor`. That way the wire physically traces the data into
 * the matching curve without relying on ImPlot's auto-cycling, which depends
 * on series-add order across the whole plot and is unstable when inputs
 * connect in different orders.
 *
 * Colors are the first four entries of seaborn / ImPlot's "Deep" palette —
 * close enough to the auto-assigned series colors that users perceive them
 * as identical, but stable per-slot regardless of how many other plots
 * exist on the canvas.
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
