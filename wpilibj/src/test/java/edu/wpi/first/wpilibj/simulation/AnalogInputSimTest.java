/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.hal.HAL;

import static org.junit.jupiter.api.Assertions.assertEquals;

class AnalogInputSimTest {
  static class DoubleStore {
    public boolean m_wasTriggered;
    public boolean m_wasCorrectType;
    public double m_setValue0;
  }

  @Test
  void setCallbackTest() {
    HAL.initialize(500, 0);

    try (AnalogInput input = new AnalogInput(5)) {
      AnalogInputSim inputSim = new AnalogInputSim(input);

      for (double i = 0; i < 5.0; i += 0.1) {
        inputSim.setVoltage(0);

        assertEquals(input.getVoltage(), 0, 0.001);

        inputSim.setVoltage(i);

        assertEquals(input.getVoltage(), i, 0.001);
      }
    }
  }
}
