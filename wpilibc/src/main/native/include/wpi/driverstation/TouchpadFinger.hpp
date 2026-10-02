// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

/**
 * Touchpad finger data from a joystick.
 */
struct TouchpadFinger final {
  /// Whether the finger is touching the touchpad.
  bool down = false;
  /// The x position of the finger on the touchpad.
  float x = 0.0f;
  /// The y position of the finger on the touchpad.
  float y = 0.0f;
};

}  // namespace wpi
