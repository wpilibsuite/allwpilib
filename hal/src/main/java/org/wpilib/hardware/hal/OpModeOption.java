// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** An individual opmode option. */
public class OpModeOption {
  /** Unique id. Encodes robot mode in bits 57-56, LSB 56 bits is hash of name. */
  @SuppressWarnings("MemberName")
  public final long id;

  @SuppressWarnings("MemberName")
  public final String name;

  @SuppressWarnings("MemberName")
  public final String group;

  @SuppressWarnings("MemberName")
  public final String description;

  @SuppressWarnings("MemberName")
  public final int textColor;

  @SuppressWarnings("MemberName")
  public final int backgroundColor;

  /**
   * Constructor.
   *
   * @param id id
   * @param name name
   * @param group group
   * @param description description
   * @param textColor text color (0x00RRGGBB or -1 for default)
   * @param backgroundColor background color (0x00RRGGBB or -1 for default)
   */
  public OpModeOption(
      long id, String name, String group, String description, int textColor, int backgroundColor) {
    this.id = id;
    this.name = name;
    this.group = group;
    this.description = description;
    this.textColor = textColor;
    this.backgroundColor = backgroundColor;
  }

  /**
   * Gets the robot mode encoded in the ID.
   *
   * @return robot mode
   */
  public RobotMode getMode() {
    return RobotMode.fromInt((int) ((id >> 56) & 0x3));
  }

  /**
   * Makes an ID from a robot mode and a hash.
   *
   * @param mode robot mode
   * @param hash hash of name
   * @return ID
   */
  public static long makeId(RobotMode mode, long hash) {
    return ((mode.getValue() & 0x3L) << 56) | (hash & 0x00FFFFFFFFFFFFFFL);
  }
}
