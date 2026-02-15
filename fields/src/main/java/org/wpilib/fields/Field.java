// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

/** An interface that exposes a method to get a field image's JSON file. */
@SuppressWarnings("PMD.ImplicitFunctionalInterface")
public interface Field {
  /**
   * Gets the field's resource file.
   *
   * @return The field's JSON resource file.
   */
  String getResourceFile();
}
