// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * ImGui panel that draws the impulse and step responses of the current biquad
 * cascade. The plots make overshoot, ringing, and settling behaviour obvious
 * — characteristics that are easy to miss in a magnitude-only Bode view.
 *
 * The cascade is run on a fresh @c BiquadFilter each frame via
 * @c ApplyFilter, so changes to the chain take effect immediately.
 */
class TimeResponseView {
 public:
  /** @param sections Current cascade (nullopt or empty clears the plot). */
  void Display(const std::optional<Sections>& sections);
};

}  // namespace wpi::filterdesigner
