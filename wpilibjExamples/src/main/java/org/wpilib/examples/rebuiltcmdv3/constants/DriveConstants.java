// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.constants;

import static org.wpilib.units.Units.FeetPerSecond;
import static org.wpilib.units.Units.RotationsPerSecond;

import org.wpilib.math.kinematics.SwerveDriveKinematics;
import org.wpilib.units.measure.AngularVelocity;
import org.wpilib.units.measure.LinearVelocity;

public final class DriveConstants {
  public static final SwerveDriveKinematics KINEMATICS = new SwerveDriveKinematics();
  public static final LinearVelocity MAX_VELOCITY = FeetPerSecond.of(15);
  public static final AngularVelocity MAX_TURN_RATE = RotationsPerSecond.of(3);

  // All drive IDs are odd, all turn IDs are even
  public static final int FRONT_LEFT_DRIVE_ID = 1;
  public static final int FRONT_LEFT_TURN_ID = 2;
  public static final int FRONT_RIGHT_DRIVE_ID = 3;
  public static final int FRONT_RIGHT_TURN_ID = 4;
  public static final int REAR_LEFT_DRIVE_ID = 5;
  public static final int REAR_LEFT_TURN_ID = 6;
  public static final int REAR_RIGHT_DRIVE_ID = 7;
  public static final int REAR_RIGHT_TURN_ID = 8;
}
