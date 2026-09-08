// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Persisted display options for a BodePlotNode. The response itself is
 * recomputed per frame from the connected Filter wires.
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
   * Log-spaced grid points. Below ~64 the curves stairstep; above ~2048 a
   * large cascade dominates the frame.
   */
  int numPoints = 512;

  /** Canvas size in pixels; plotHeight covers both stacked subplots. */
  float plotWidth = 600.0f;
  float plotHeight = 360.0f;

  static constexpr float kMinPlotWidth = 240.0f;
  static constexpr float kMinPlotHeight = 180.0f;
  static constexpr int kMinPoints = 32;
  static constexpr int kMaxPoints = 4096;
};

}  // namespace wpi::filterdesigner
