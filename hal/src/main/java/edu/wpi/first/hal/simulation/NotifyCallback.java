/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.HALValue;

public interface NotifyCallback {
  void callback(String name, HALValue value);

  default void callbackNative(String name, int type, long value1, double value2) {
    callback(name, HALValue.fromNative(type, value1, value2));
  }
}
