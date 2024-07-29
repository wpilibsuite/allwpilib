// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/DigitalInput.h>
#include <frc/DigitalOutput.h>

#include <memory>

#include <units/time.h>

namespace frc {

/**
 * This class represents the onboard IO of the XRP
 * reference robot. This the USER push button and
 * LED.
 *
 * <p>DIO 0 - USER Button (input only)
 * DIO 1 - LED (output only)
 */
class XRPOnBoardIO {
 public:
  XRPOnBoardIO() {}  // No need to do anything. No configurable IO

  static constexpr auto kMessageInterval = 1_s;
  units::second_t m_nextMessageTime = 0_s;

  /**
   * Gets if the USER button is pressed.
   */
  bool GetUserButtonPressed();

  /**
   * Sets the yellow LED.
   */
  void SetLed(bool value);

 private:
  frc::DigitalInput m_userButton{0};
  frc::DigitalOutput m_led{1};
};

}  // namespace frc
