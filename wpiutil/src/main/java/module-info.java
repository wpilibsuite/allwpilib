// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib utility module. This defines miscellaneous APIs for concurrency, data logging, lambda
 * functions, binary serialization and deserialization, native library interactions, and error
 * reporting.
 */
@SuppressWarnings("requires-transitive-automatic")
open module wpilib.util {
  requires com.fasterxml.jackson.databind;
  requires transitive us.hebi.quickbuf.runtime;

  exports edu.wpi.first.util;
  exports edu.wpi.first.util.cleanup;
  exports edu.wpi.first.util.concurrent;
  exports edu.wpi.first.util.function;
  exports edu.wpi.first.util.protobuf;
  exports edu.wpi.first.util.sendable;
  exports edu.wpi.first.util.struct;
}
