// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation.testutils;

import org.wpilib.hardware.hal.HALValue;

public class EnumCallback extends CallbackHelperBase<Long> {
  @Override
  public void callback(String name, HALValue value) {
    if (value.getType() != HALValue.kEnum) {
      throw new IllegalArgumentException("Wrong callback for type " + value.getType());
    }

    m_wasTriggered = true;
    m_setValue = value.getLong();
  }
}
