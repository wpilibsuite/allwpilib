// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) configuration for a PoleZeroPlotNode. Poles + zeros are
 * recomputed per-frame from the connected Filter wires via @ref
 * ComputePolesZeros; this struct only holds persisted display options.
 */
struct PoleZeroPlotNodeLogic {
  /** If true (default) the legend overlay is shown. */
  bool showLegend = true;

  /**
   * Plot canvas size in screen pixels. The plot uses an equal-aspect view
   * (1 Re unit == 1 Im unit), so changing width/height changes how much of
   * the plane is visible without distorting the unit circle.
   */
  float plotWidth = 360.0f;
  float plotHeight = 360.0f;

  static constexpr float kMinPlotWidth = 200.0f;
  static constexpr float kMinPlotHeight = 200.0f;
};

}  // namespace wpi::filterdesigner
