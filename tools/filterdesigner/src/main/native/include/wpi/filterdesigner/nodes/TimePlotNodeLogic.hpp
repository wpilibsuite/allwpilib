// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Persisted display options for a TimePlotNode, kept out of the node so they
 * round-trip through .fdsgn v2 and can be tested without ImGui.
 */
struct TimePlotNodeLogic {
  /**
   * If true (default) the y-axis rescales to the data each frame. Off lets
   * users zoom + lock a range manually.
   */
  bool autoscale = true;

  /** If true (default) the legend overlay is shown. */
  bool showLegend = true;

  /** Canvas size in pixels. The node auto-sizes to it. */
  float plotWidth = 600.0f;
  float plotHeight = 320.0f;

  /** Lower bounds for the drag-resize grip — anything smaller is unreadable. */
  static constexpr float kMinPlotWidth = 240.0f;
  static constexpr float kMinPlotHeight = 140.0f;
};

}  // namespace wpi::filterdesigner
