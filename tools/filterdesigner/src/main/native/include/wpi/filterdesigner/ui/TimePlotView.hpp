// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include "wpi/filterdesigner/model/Signal.hpp"

namespace wpi::filterdesigner {

/**
 * ImGui view that renders a @ref Signal as a time-domain line plot, with an
 * optional second line overlaid (typically the signal run through the current
 * filter design).
 *
 * Stateless; caller supplies the signal and any overlay each frame.
 */
class TimePlotView {
 public:
  /**
   * Draw the plot. Must be inside an ImGui window.
   *
   * @param signal  Raw signal; may be null.
   * @param filtered If non-empty, drawn as a second line with the same
   *                 timestamps as @a signal. Must match signal length or be
   *                 empty.
   */
  void Display(const Signal* signal, std::span<const double> filtered = {});
};

}  // namespace wpi::filterdesigner
