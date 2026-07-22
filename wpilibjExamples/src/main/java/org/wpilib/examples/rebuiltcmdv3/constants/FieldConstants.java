// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.constants;

import java.util.Optional;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rectangle2d;
import org.wpilib.math.geometry.Translation2d;

public final class FieldConstants {
  // TODO: Measurements
  public static final Pose2d RED_HUB_CENTER = new Pose2d();
  public static final Pose2d BLUE_HUB_CENTER = new Pose2d();
  public static final Rectangle2d NEUTRAL_ZONE =
      new Rectangle2d(new Translation2d(), new Translation2d());
  public static final Rectangle2d RED_ZONE =
      new Rectangle2d(new Translation2d(), new Translation2d());
  public static final Rectangle2d BLUE_ZONE =
      new Rectangle2d(new Translation2d(), new Translation2d());

  /**
   * Gets location of our hub. Returns an empty optional if we don't have an alliance yet.
   *
   * @return The location of our hub.
   */
  public static Optional<Pose2d> targetHub() {
    return DriverStationBackend.getAlliance()
        .map(
            a -> {
              return switch (a) {
                case RED -> RED_HUB_CENTER;
                case BLUE -> BLUE_HUB_CENTER;
              };
            });
  }
}
