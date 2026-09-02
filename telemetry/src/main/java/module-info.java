// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * A unified, extensible, data-oriented, output-only telemetry API for WPILib with hierarchical
 * structure and flexible backend implementation.
 */
module wpilib.telemetry {
  requires transitive wpilib.util;

  exports org.wpilib.telemetry;
  exports org.wpilib.telemetry.util;
}
