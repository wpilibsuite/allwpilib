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

  exports edu.wpi.first.hal;
  exports edu.wpi.first.hal.can;
  exports edu.wpi.first.hal.communication;
  exports edu.wpi.first.hal.simulation;
  exports edu.wpi.first.hal.util;
}
