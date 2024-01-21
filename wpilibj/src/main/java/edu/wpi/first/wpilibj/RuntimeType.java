// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HALUtil;

/** Runtime type. */
public enum RuntimeType {
  /** roboRIO 1.0. */
  kRoboRIO(HALUtil.RUNTIME_ROBORIO),
  /** roboRIO 2.0. */
  kRoboRIO2(HALUtil.RUNTIME_ROBORIO2),
  /** Simulation runtime. */
  kSimulation(HALUtil.RUNTIME_SIMULATION);

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
      case HALUtil.RUNTIME_ROBORIO -> RuntimeType.kRoboRIO;
      case HALUtil.RUNTIME_ROBORIO2 -> RuntimeType.kRoboRIO2;
      default -> RuntimeType.kSimulation;
    };
  }
}
