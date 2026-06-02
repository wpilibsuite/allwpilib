// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) configuration for a TimePlotNode. The plot itself is pure
 * UI — the connected pins feed it data every frame — but the persisted
 * options (legend on/off, autoscale, etc.) live here so they round-trip
 * through .fdsgn v2 and can be unit-tested as a config struct.
 *
 * Kept intentionally tiny; nodes are expected to grow their @c Logic struct
 * as features land.
 */
struct TimePlotNodeLogic {
  /**
   * If true (default) the y-axis rescales to the data each frame. Off lets
   * users zoom + lock a range manually.
   */
  bool autoscale = true;

  /** If true (default) the legend overlay is shown. */
  bool showLegend = true;

  /**
   * Plot canvas size, in screen pixels. ImNodeFlow nodes auto-size to their
   * content, so changing these resizes the entire node. Persisted so the
   * user's drag-resize survives save/reload.
   */
  float plotWidth = 600.0f;
  float plotHeight = 320.0f;

  /** Lower bounds for the drag-resize grip — anything smaller is unreadable. */
  static constexpr float kMinPlotWidth = 240.0f;
  static constexpr float kMinPlotHeight = 140.0f;
};

}  // namespace wpi::filterdesigner
