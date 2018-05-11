/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

public interface NotifyCallback {
  void callback(String name, SimValue value);

  default void callbackNative(String name, int type, long value1, double value2) {
    switch(type) {
      case 0x01:
        callback(name, SimValue.makeBoolean(value1 != 0));
      break;
      case 0x02:
        callback(name, SimValue.makeDouble(value2));
      break;
      case 0x16:
        callback(name, SimValue.makeEnum((int)value1));
      break;
      case 0x32:
        callback(name, SimValue.makeInt((int)value1));
      break;
      case 0x64:
        callback(name, SimValue.makeLong(value1));
        break;
      default:
        callback(name, SimValue.makeUnassigned());
    }
  }
}
