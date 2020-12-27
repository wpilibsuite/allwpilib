/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

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
