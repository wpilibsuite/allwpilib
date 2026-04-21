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
  CENTER(0x00),
  /** POV up. */
  UP(0x01),
  /** POV up right. */
  UP_RIGHT(0x01 | 0x02),
  /** POV right. */
  RIGHT(0x02),
  /** POV down right. */
  DOWN_RIGHT(0x02 | 0x04),
  /** POV down. */
  DOWN(0x04),
  /** POV down left. */
  DOWN_LEFT(0x04 | 0x08),
  /** POV left. */
  LEFT(0x08),
  /** POV up left. */
  UP_LEFT(0x01 | 0x08);

  private static final double INVALID_POV_VALUE_INTERVAL = 1.0;
  private static double s_nextMessageTime;

  /**
   * Converts a byte value into a POVDirection enum value.
   *
   * @param value The byte value to convert.
   * @return The corresponding POVDirection enum value.
   * @throws IllegalArgumentException If value does not correspond to a POVDirection.
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
    return CENTER;
  }

  /** The corresponding HAL value. */
  public final byte value;

  POVDirection(int value) {
    this.value = (byte) value;
  }

  /**
   * Gets the angle of a POVDirection.
   *
   * @return The angle clockwise from straight up, or Optional.empty() if this POVDirection is
   *     CENTER.
   */
  public Optional<Rotation2d> getAngle() {
    return switch (this) {
      case CENTER -> Optional.empty();
      case UP -> Optional.of(Rotation2d.fromDegrees(0));
      case UP_RIGHT -> Optional.of(Rotation2d.fromDegrees(45));
      case RIGHT -> Optional.of(Rotation2d.fromDegrees(90));
      case DOWN_RIGHT -> Optional.of(Rotation2d.fromDegrees(135));
      case DOWN -> Optional.of(Rotation2d.fromDegrees(180));
      case DOWN_LEFT -> Optional.of(Rotation2d.fromDegrees(225));
      case LEFT -> Optional.of(Rotation2d.fromDegrees(270));
      case UP_LEFT -> Optional.of(Rotation2d.fromDegrees(315));
    };
  }
}
