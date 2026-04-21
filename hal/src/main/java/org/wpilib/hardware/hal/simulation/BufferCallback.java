// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.simulation;

/** Interface for simulation buffer callbacks. */
@FunctionalInterface
public interface BufferCallback {
  /**
   * Simulation buffer callback function.
   *
   * @param name Buffer name.
   * @param buffer Buffer.
   * @param count Buffer size.
   */
  void callback(String name, byte[] buffer, int count);
}
