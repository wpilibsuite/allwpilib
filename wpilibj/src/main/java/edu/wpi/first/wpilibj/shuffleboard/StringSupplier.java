/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * Represents a supplier of {@link String Strings} for use in updating the value
 * of a Shuffleboard widget.
 */
@FunctionalInterface
public interface StringSupplier {
  /**
   * Gets the String to send to the widget.
   *
   * @return the value to be sent
   */
  String get();
}
