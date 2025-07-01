// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.fields;

/**
 * An interface that exposes a method to get a field image's JSON file.
 */
public interface Field {
 /**
  * Gets the field's resource file.
  *
  * @return The field's JSON resource file.
  */
  String getResourceFile();
}
