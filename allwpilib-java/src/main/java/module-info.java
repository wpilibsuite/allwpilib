// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib module. This provides no APIs or libraries, but has transitive requirements for every
 * WPILib submodule, such as {@code wpilib.core} and {@link wpilib.math}. Robot programs can require
 * just the `wpilib` module to get access to all the WPILib APIs. Submodules that are distributed as
 * vendor deps - like the command framework - are <i>not</i> included, and must be required
 * separately.
 *
 * <p>An example robot program's {@code module-info.java} file could look like this:</p>
 * {@snippet lang="java":
 * module robot {
 *   requires transitive wpilib;
 *   requires transitive wpilib.commands2; // if using the v2 framework
 *   requires transitive wpilib.commands3; // if using the v3 framework
 *
 *   exports frc.robot;
 *   exports frc.robot.constants;
 *   exports frc.robot.subsystems;
 * }
 * }
 *
 * Such a module setup allows for a single import line to access all top level classes from the
 * WPILib libraries and your own robot program!
 *
 * {@snippet lang="java":
 * import module robot;
 *
 * public class Robot extends TimedRobot { // imported from edu.wpi.first.wpilibj
 *   private Drivetrain drivetrain; // imported from frc.robot.subsystems
 * }
 * }
 */
open module wpilib {
  requires transitive wpilib.cameraserver;
  requires transitive wpilib.core;
  requires transitive wpilib.cscore;
  requires transitive wpilib.epilogue;
  requires transitive wpilib.hal;
  requires transitive wpilib.math;
  requires transitive wpilib.ntcore;
  requires transitive wpilib.units;
  requires transitive wpilib.util;
}
