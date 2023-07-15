// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation.testutils;

import edu.wpi.first.hal.HALValue;

public class IntCallback extends CallbackHelperBase<Integer> {
  @Override
  public void callback(String name, HALValue value) {
    if (value.getType() != HALValue.kInt) {
      throw new IllegalArgumentException("Wrong callback for type " + value.getType());
    }

    m_wasTriggered = true;
    m_setValue = (int) value.getLong();
  }
}
