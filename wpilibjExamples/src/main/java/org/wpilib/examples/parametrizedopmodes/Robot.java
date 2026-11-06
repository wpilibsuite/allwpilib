// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.parametrizedopmodes;

import java.io.File;
import java.io.IOException;
import java.io.UncheckedIOException;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.math.trajectory.DifferentialTrajectory;
import org.wpilib.system.Filesystem;

/**
 * Demonstrates registering several opmodes of the same class with different constructor parameters.
 * Each autonomous opmode follows a different trajectory loaded from the deploy directory.
 */
public class Robot extends OpModeRobot {
  /** The robot's drivetrain, shared by all of the trajectory opmodes. */
  public final Drivetrain drivetrain = new Drivetrain();

  /** Creates the robot and registers a trajectory opmode for each trajectory file. */
  public Robot() {
    // Opmode classes found by the annotation scan can only be constructed with no arguments or with
    // the robot. To give each opmode extra constructor arguments, register a factory that captures
    // them instead.
    var straight = loadTrajectory("straight.json");
    addOpMode(
        RobotMode.AUTONOMOUS,
        "Straight Line",
        "Trajectories",
        "Drives straight forward for 2 meters",
        () -> new TrajectoryOpMode(this, straight));

    var arc = loadTrajectory("arc.json");
    addOpMode(
        RobotMode.AUTONOMOUS,
        "Arc",
        "Trajectories",
        "Drives along a left-curving arc",
        () -> new TrajectoryOpMode(this, arc));

    publishOpModes();
  }

  /** Loads a trajectory from the deploy directory's "trajectories" folder. */
  private static DifferentialTrajectory loadTrajectory(String fileName) {
    var file = new File(new File(Filesystem.getDeployDirectory(), "trajectories"), fileName);
    try {
      return DifferentialTrajectory.loadFromFile(file);
    } catch (IOException e) {
      throw new UncheckedIOException("Could not read trajectory file " + file, e);
    }
  }
}
