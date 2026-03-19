// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import org.wpilib.hardware.hal.HALUtil;

/** Runtime type. */
public enum RuntimeType {
  /** roboRIO 1.0. */
  ROBORIO(HALUtil.RUNTIME_ROBORIO),
  /** roboRIO 2.0. */
  ROBORIO_2(HALUtil.RUNTIME_ROBORIO_2),
  /** Simulation runtime. */
  SIMULATION(HALUtil.RUNTIME_SIMULATION),
  /** Systemcore. */
  SYSTEMCORE(HALUtil.RUNTIME_SYSTEMCORE);

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
    return switch (type) {
      case HALUtil.RUNTIME_ROBORIO -> RuntimeType.ROBORIO;
      case HALUtil.RUNTIME_ROBORIO_2 -> RuntimeType.ROBORIO_2;
      case HALUtil.RUNTIME_SYSTEMCORE -> RuntimeType.SYSTEMCORE;
      default -> RuntimeType.SIMULATION;
    };
  }
}
