// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

@SuppressWarnings("requires-transitive-automatic")
open module wpilib.command3 {
  requires transitive us.hebi.quickbuf.runtime;
  requires transitive wpilib.units;
  requires wpilib.annotation;
  requires wpilib.core;
  requires wpilib.hal;
  requires wpilib.math;
  requires wpilib.ntcore;
  requires wpilib.util;

  exports org.wpilib.command3;
  exports org.wpilib.command3.proto;
  exports org.wpilib.command3.button;
}
