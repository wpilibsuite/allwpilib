// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.HALValue;

@FunctionalInterface
public interface SimValueCallback {
  void callback(String name, int handle, int direction, HALValue value);

  default void callbackNative(
      String name, int handle, int direction, int type, long value1, double value2) {
    callback(name, handle, direction, HALValue.fromNative(type, value1, value2));
  }
}
