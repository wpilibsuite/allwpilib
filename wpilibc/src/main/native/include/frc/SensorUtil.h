// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/**
 * Stores most recent status information as well as containing utility functions
 * for checking channels and error processing.
 */
class SensorUtil final {
 public:
  SensorUtil() = delete;

  /**
   * Get the number of the default solenoid module.
   *
   * @return The number of the default solenoid module.
   */
  static int GetDefaultCTREPCMModule();

  /**
   * Get the number of the default solenoid module.
   *
   * @return The number of the default solenoid module.
   */
  static int GetDefaultREVPHModule();

  /**
   * Check that the digital channel number is valid.
   *
   * Verify that the channel number is one of the legal channel numbers. Channel
   * numbers are 0-based.
   *
   * @return Digital channel is valid
   */
  static bool CheckDigitalChannel(int channel);

  /**
   * Check that the relay channel number is valid.
   *
   * Verify that the channel number is one of the legal channel numbers. Channel
   * numbers are 0-based.
   *
   * @return Relay channel is valid
   */
  static bool CheckRelayChannel(int channel);

  /**
   * Check that the digital channel number is valid.
   *
   * Verify that the channel number is one of the legal channel numbers. Channel
   * numbers are 0-based.
   *
   * @return PWM channel is valid
   */
  static bool CheckPWMChannel(int channel);

  /**
   * Check that the analog input number is value.
   *
   * Verify that the analog input number is one of the legal channel numbers.
   * Channel numbers are 0-based.
   *
   * @return Analog channel is valid
   */
  static bool CheckAnalogInputChannel(int channel);

  /**
   * Check that the analog output number is valid.
   *
   * Verify that the analog output number is one of the legal channel numbers.
   * Channel numbers are 0-based.
   *
   * @return Analog channel is valid
   */
  static bool CheckAnalogOutputChannel(int channel);

  static int GetNumDigitalChannels();
  static int GetNumAnalogInputs();
  static int GetNumAnalogOuputs();
  static int GetNumPwmChannels();
  static int GetNumRelayChannels();
};

}  // namespace frc
