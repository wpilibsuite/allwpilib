// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/SubsystemBase.h>
#include <frc/DigitalInput.h>
#include <frc/DigitalOutput.h>

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
class OnBoardIO : public frc2::SubsystemBase {
 public:
  enum ChannelMode { INPUT, OUTPUT };
  OnBoardIO(OnBoardIO::ChannelMode dio1, ChannelMode dio2);

  static constexpr double MESSAGE_INTERVAL = 1.0;
  double m_nextMessageTime;

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
  frc::DigitalInput m_buttonA{0};
  frc::DigitalOutput m_yellowLed{3};

  // DIO 1
  frc::DigitalInput m_buttonB;
  frc::DigitalOutput m_greenLed;

  // DIO 2
  frc::DigitalInput m_buttonC;
  frc::DigitalOutput m_redLed;
};
