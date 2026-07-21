// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib HAL module. This defines APIs for interacting with hardware through the native C++
 * layer and JNI bindings. Most robot programs don't need to use this directly and should use the
 * {@code wpilib} module instead.
 */
open module wpilib.hal {
  requires wpilib.util;

  exports org.wpilib.hardware.hal;
  exports org.wpilib.hardware.hal.can;
  exports org.wpilib.hardware.hal.simulation;
  exports org.wpilib.hardware.hal.struct;
  exports org.wpilib.hardware.hal.util;
}
