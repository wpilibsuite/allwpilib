// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) configuration for a FrequencyPlotNode. The spectrum itself
 * is recomputed per-frame from the connected Signal wires via @ref
 * Spectrum::Compute; this struct only holds persisted display options.
 */
struct FrequencyPlotNodeLogic {
  /** If true (default) the y-axis rescales to the data each frame. */
  bool autoscale = true;

  /** If true (default) the legend overlay is shown. */
  bool showLegend = true;

  /**
   * If true (default), x-axis is log-scaled. Off = linear; matches what the
   * linear-chain @c FrequencyPlotView uses out of the box.
   */
  bool logFrequency = false;

  /**
   * Plot canvas size in screen pixels. ImNodeFlow nodes auto-size to their
   * content, so changing these resizes the node body. Persisted so the
   * user's drag-resize survives save/reload.
   */
  float plotWidth = 600.0f;
  float plotHeight = 320.0f;

  static constexpr float kMinPlotWidth = 240.0f;
  static constexpr float kMinPlotHeight = 140.0f;
};

}  // namespace wpi::filterdesigner
