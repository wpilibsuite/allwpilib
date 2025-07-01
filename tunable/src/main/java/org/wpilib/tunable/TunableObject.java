// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/** The base interface for complex tunable objects. */
public interface TunableObject {
  /**
   * Gets the tunable table type. Default is no specified table type (null).
   *
   * @return Table type
   */
  default String getTunableType() {
    return null;
  }

  void initTunable(TunableTable table);
}
