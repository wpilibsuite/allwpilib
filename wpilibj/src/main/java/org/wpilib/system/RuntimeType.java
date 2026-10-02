// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import org.wpilib.hardware.hal.HALUtil;

/** Runtime type. */
public enum RuntimeType {
  /** Systemcore runtime. */
  SYSTEMCORE(HALUtil.RUNTIME_SYSTEMCORE),
  /** Simulation runtime. */
  SIMULATION(HALUtil.RUNTIME_SIMULATION);

  /** RuntimeType value. */
  public final int value;

  RuntimeType(int value) {
    this.value = value;
  }

  /**
   * Construct a runtime type from an int value.
   *
   * @param type Runtime type as int
   * @return Matching runtime type
   */
  public static RuntimeType getValue(int type) {
    if (type == HALUtil.RUNTIME_SYSTEMCORE) {
      return RuntimeType.SYSTEMCORE;
    } else {
      return RuntimeType.SIMULATION;
    }
  }
}
