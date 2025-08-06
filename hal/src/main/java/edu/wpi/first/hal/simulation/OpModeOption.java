// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

public class OpModeOption {
  public final long id;
  public final String name;
  public final String group;
  public final String description;
  public final int textColor;
  public final int backgroundColor;

  public OpModeOption(long id, String name, String group, String description, int textColor, int backgroundColor) {
    this.id = id;
    this.name = name;
    this.group = group;
    this.description = description;
    this.textColor = textColor;
    this.backgroundColor = backgroundColor;
  }
}
