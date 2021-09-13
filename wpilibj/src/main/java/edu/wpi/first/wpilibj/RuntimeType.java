// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HALUtil;

public enum RuntimeType {
    kRoboRIO(HALUtil.RUNTIME_ROBORIO),
    kRoboRIO2(HALUtil.RUNTIME_ROBORIO2),
    kSimulation(HALUtil.RUNTIME_SIMULATION);

    public final int value;

    RuntimeType(int value) {
      this.value = value;
    }

    public static RuntimeType getValue(int type) {
        if (type == HALUtil.RUNTIME_ROBORIO) {
            return RuntimeType.kRoboRIO;
        } else if (type == HALUtil.RUNTIME_ROBORIO2) {
            return RuntimeType.kRoboRIO;
        }
        return RuntimeType.kSimulation;
    }
}
