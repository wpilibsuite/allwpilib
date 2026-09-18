// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib module. This provides no APIs or libraries, but has transitive requirements for every
 * WPILib submodule, such as {@link wpilib.core} and {@link wpilib.math}. Robot programs can require
 * just the {@code wpilib} module to get access to all the WPILib APIs. Submodules that are
 * distributed as vendor deps - like the command frameworks - are <i>not</i> included, and must be
 * required separately.
 *
 * <p>Note that robot programs do not need a {@code module-info.java} file to access WPILib APIs.
 * GradleRIO automatically places the WPILib libraries - and any other library that has Java module
 * information - onto the module path at compile time. <strong>Robot programs can still use import
 * statements like {@code import module wpilib} or {@code import module wpilib.command3} without a
 * {@code module-info.java} file.</strong>
 *
 * <p>However, adding a {@code module-info.java} file allows team code to be imported as well. An
 * example robot program's {@code module-info.java} file could look like this:
 *
 * {@snippet lang="java":
 * module robot {
 *   requires transitive wpilib;
 *   requires transitive wpilib.command2; // if using the v2 framework
 *   requires transitive wpilib.command3; // if using the v3 framework
 *
 *   exports first.robot;
 *   exports first.robot.constants;
 *   exports first.robot.subsystems;
 * }
 * }
 *
 * Such a module setup allows for a single import line to access all top level classes from the
 * WPILib libraries and your own robot program!
 *
 * {@snippet lang="java":
 * import module robot;
 *
 * public class Robot extends TimedRobot { // imported from org.wpilib.framework
 *   private Drivetrain drivetrain; // imported from first.robot.mechanisms
 * }
 * }
 */
open module wpilib {
  requires transitive wpilib.core;
  requires transitive wpilib.epilogue;
  requires transitive wpilib.hal;
  requires transitive wpilib.math;
  requires transitive wpilib.ntcore;
  requires transitive wpilib.telemetry;
  requires transitive wpilib.tunable;
  requires transitive wpilib.units;
  requires transitive wpilib.util;
}
