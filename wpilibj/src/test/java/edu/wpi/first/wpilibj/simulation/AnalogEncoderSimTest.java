// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.AnalogEncoder;
import edu.wpi.first.wpilibj.AnalogInput;
import org.junit.jupiter.api.Test;

class AnalogEncoderSimTest {
  @Test
  void testBasic() {
    try (var analogInput = new AnalogInput(0);
        var analogEncoder = new AnalogEncoder(analogInput, 360, 0)) {
      var encoderSim = new AnalogEncoderSim(analogEncoder);

      encoderSim.set(180);
      assertEquals(analogEncoder.get(), 180, 1E-8);
      assertEquals(encoderSim.get(), 180, 1E-8);
    }
  }
}
