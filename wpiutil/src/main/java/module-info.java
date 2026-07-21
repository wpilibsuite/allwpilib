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
  requires io.avaje.json;
  requires transitive us.hebi.quickbuf.runtime;

  exports org.wpilib.util;
  exports org.wpilib.util.cleanup;
  exports org.wpilib.util.concurrent;
  exports org.wpilib.util.container;
  exports org.wpilib.util.function;
  exports org.wpilib.util.protobuf;
  exports org.wpilib.util.runtime;
  exports org.wpilib.util.sendable;
  exports org.wpilib.util.struct;
}
