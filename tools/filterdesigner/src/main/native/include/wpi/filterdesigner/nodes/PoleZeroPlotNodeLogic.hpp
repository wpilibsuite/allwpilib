// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Persisted display options for a PoleZeroPlotNode. The roots themselves are
 * recomputed per frame from the connected Filter wires.
 */
struct PoleZeroPlotNodeLogic {
  /** If true (default) the legend overlay is shown. */
  bool showLegend = true;

  /**
   * Canvas size in pixels. The view is equal-aspect, so this changes how much
   * of the plane is visible rather than distorting the unit circle.
   */
  float plotWidth = 360.0f;
  float plotHeight = 360.0f;

  static constexpr float kMinPlotWidth = 200.0f;
  static constexpr float kMinPlotHeight = 200.0f;
};

}  // namespace wpi::filterdesigner
