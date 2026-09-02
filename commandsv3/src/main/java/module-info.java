// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/** Defines APIs for coroutine-based, event-driven command scheduling and mechanism coordination. */
open module wpilib.command3 {
  requires transitive us.hebi.quickbuf.runtime;
  requires transitive wpilib.core;
  requires transitive wpilib.ntcore;
  requires transitive wpilib.units;
  requires transitive wpilib.util;
  requires wpilib.annotation;
  requires wpilib.hal;
  requires wpilib.math;

  exports org.wpilib.command3;
  exports org.wpilib.command3.proto;
  exports org.wpilib.command3.button;
}
