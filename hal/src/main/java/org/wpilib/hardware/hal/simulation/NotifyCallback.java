// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.simulation;

import org.wpilib.hardware.hal.HALValue;

@FunctionalInterface
public interface NotifyCallback {
  void callback(String name, HALValue value);

  default void callbackNative(String name, int type, long value1, double value2) {
    callback(name, HALValue.fromNative(type, value1, value2));
  }
}
