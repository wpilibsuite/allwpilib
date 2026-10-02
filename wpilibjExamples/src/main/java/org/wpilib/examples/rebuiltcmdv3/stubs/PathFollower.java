// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.stubs;

import org.wpilib.math.kinematics.ChassisVelocities;

/**
 * A stub interface for an API that could plausibly follow a path. No WPILib code exists for this;
 * if you want to follow paths on a real robot, use a third-party library provided by the FRC
 * programming community.
 */
public interface PathFollower {
  /**
   * Gets the next chassis velocities from the path.
   *
   * @return the next chassis velocities
   */
  ChassisVelocities next();

  /**
   * Checks if the robot has reached the end of the path.
   *
   * @return true if the robot has reached the end of the path, false otherwise
   */
  boolean isDone();

  /**
   * Loads a path from a file in the deploy directory. This is a stub implementation as an example;
   * a real robot program would use a third-party path following library.
   *
   * @param pathName The name of the path file to load.
   * @return A PathFollower instance that can follow the loaded path.
   */
  static PathFollower load(String pathName) {
    return new PathFollower() {
      @Override
      public ChassisVelocities next() {
        return new ChassisVelocities();
      }

      @Override
      public boolean isDone() {
        return true;
      }
    };
  }
}
