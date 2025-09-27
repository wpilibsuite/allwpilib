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
 * @ingroup xrp_api
 * @{
 */

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
   *
   * @return True if the USER button is currently pressed.
   */
  bool GetUserButtonPressed();

  /**
   * Sets the yellow LED.
   *
   * @param value True to activate LED, false otherwise.
   */
  void SetLed(bool value);

  /**
   * Gets the state of the yellow LED.
   *
   * @return True if LED is active, false otherwise.
   */
  bool GetLed() const;

 private:
  frc::DigitalInput m_userButton{0};
  frc::DigitalOutput m_led{1};
};

/** @} */

}  // namespace frc
