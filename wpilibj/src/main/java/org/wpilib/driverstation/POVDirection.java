// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.Optional;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.system.Timer;

/** A controller POV direction. */
public enum POVDirection {
  /** POV center. */
  Center(0x00),
  /** POV up. */
  Up(0x01),
  /** POV up right. */
  UpRight(0x01 | 0x02),
  /** POV right. */
  Right(0x02),
  /** POV down right. */
  DownRight(0x02 | 0x04),
  /** POV down. */
  Down(0x04),
  /** POV down left. */
  DownLeft(0x04 | 0x08),
  /** POV left. */
  Left(0x08),
  /** POV up left. */
  UpLeft(0x01 | 0x08);

  private static final double INVALID_POV_VALUE_INTERVAL = 1.0;
  private static double s_nextMessageTime;

  /**
   * Converts a byte value into a POVDirection enum value.
   *
   * @param value The byte value to convert.
   * @return The corresponding POVDirection enum value.
   * @throws IllegalArgumentException If value does not correspond to a
   *                                  POVDirection.
   */
  public static POVDirection of(byte value) {
    for (var direction : values()) {
      if (direction.value == value) {
        return direction;
      }
    }
    double currentTime = Timer.getTimestamp();
    if (currentTime > s_nextMessageTime) {
      DriverStationErrors.reportError("Invalid POV value " + value + "!", false);
      s_nextMessageTime = currentTime + INVALID_POV_VALUE_INTERVAL;
    }
    return Center;
  }

  /** The corresponding HAL value. */
  public final byte value;

  private POVDirection(int value) {
    this.value = (byte) value;
  }

  /**
   * Gets the angle of a POVDirection.
   *
   * @return The angle clockwise from straight up, or Optional.empty() if this
   *         POVDirection is
   *         Center.
   */
  public Optional<Rotation2d> getAngle() {
    return switch (this) {
      case Center -> Optional.empty();
      case Up -> Optional.of(Rotation2d.fromDegrees(0));
      case UpRight -> Optional.of(Rotation2d.fromDegrees(45));
      case Right -> Optional.of(Rotation2d.fromDegrees(90));
      case DownRight -> Optional.of(Rotation2d.fromDegrees(135));
      case Down -> Optional.of(Rotation2d.fromDegrees(180));
      case DownLeft -> Optional.of(Rotation2d.fromDegrees(225));
      case Left -> Optional.of(Rotation2d.fromDegrees(270));
      case UpLeft -> Optional.of(Rotation2d.fromDegrees(315));
    };
  }
}
