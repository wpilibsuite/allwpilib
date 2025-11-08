// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/discrete/DigitalOutput.hpp"
#include "wpi/units/time.hpp"

namespace wpi::romi {

/**
 * @ingroup romi_api
 * @{
 */

/**
 * This class represents the onboard IO of the Romi
 * reference robot. This includes the pushbuttons and
 * LEDs.
 *
 * <p>DIO 0 - Button A (input only)
 * DIO 1 - Button B (input) or Green LED (output)
 * DIO 2 - Button C (input) or Red LED (output)
 * DIO 3 - Yellow LED (output only)
 */
class OnBoardIO {
 public:
  /** Mode for Romi onboard IO */
  enum ChannelMode { /** Input */ INPUT, /** Output */ OUTPUT };
  OnBoardIO(OnBoardIO::ChannelMode dio1, OnBoardIO::ChannelMode dio2);

  static constexpr auto kMessageInterval = 1_s;
  wpi::units::second_t m_nextMessageTime = 0_s;

  /**
   * Gets if the A button is pressed.
   */
  bool GetButtonAPressed();

  /**
   * Gets if the B button is pressed.
   */
  bool GetButtonBPressed();

  /**
   * Gets if the C button is pressed.
   */
  bool GetButtonCPressed();

  /**
   * Sets the green LED.
   */
  void SetGreenLed(bool value);

  /**
   * Sets the red LED.
   */
  void SetRedLed(bool value);

  /**
   * Sets the yellow LED.
   */
  void SetYellowLed(bool value);

 private:
  wpi::DigitalInput m_buttonA{0};
  wpi::DigitalOutput m_yellowLed{3};

  // DIO 1
  std::unique_ptr<wpi::DigitalInput> m_buttonB;
  std::unique_ptr<wpi::DigitalOutput> m_greenLed;

  // DIO 2
  std::unique_ptr<wpi::DigitalInput> m_buttonC;
  std::unique_ptr<wpi::DigitalOutput> m_redLed;
};

/** @} */

}  // namespace wpi::romi
