// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

/** An individual opmode option. */
public class OpModeOption {
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
   * @param textColor text color (RGB)
   * @param backgroundColor background color (RGB)
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
}
