// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) configuration for a BodePlotNode. The actual frequency
 * response is recomputed per-frame from the connected Filter wires via
 * @ref FrequencyResponse::Compute; this struct holds only the persisted
 * display options.
 */
struct BodePlotNodeLogic {
  /**
   * If true (default) the magnitude/phase axes rescale to the data each
   * frame. Off lets users zoom + lock manually.
   */
  bool autoscale = true;

  /** If true (default) the legend overlay is shown. */
  bool showLegend = true;

  /**
   * Number of log-spaced grid points used by @ref FrequencyResponse::Compute.
   * Below ~64 the curves look stairstepped; above ~2048 the cost dominates
   * the frame for large cascades.
   */
  int numPoints = 512;

  /**
   * Plot canvas size in screen pixels. The plot is two stacked subplots
   * (magnitude + phase); plotHeight is the *total* canvas height split
   * across both.
   */
  float plotWidth = 600.0f;
  float plotHeight = 360.0f;

  static constexpr float kMinPlotWidth = 240.0f;
  static constexpr float kMinPlotHeight = 180.0f;
  static constexpr int kMinPoints = 32;
  static constexpr int kMaxPoints = 4096;
};

}  // namespace wpi::filterdesigner
