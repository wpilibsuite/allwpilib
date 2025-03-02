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

  static int GetNumSmartIoPorts();
};

}  // namespace frc
