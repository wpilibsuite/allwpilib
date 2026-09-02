// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/** Defines APIs for writing and reading WPILib data logs. */
open module wpilib.datalog {
  requires transitive us.hebi.quickbuf.runtime;
  requires transitive wpilib.util;

  exports org.wpilib.datalog;
}
