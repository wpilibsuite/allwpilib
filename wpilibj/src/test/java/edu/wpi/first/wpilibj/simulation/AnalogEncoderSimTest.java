/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.AnalogEncoder;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class AnalogEncoderSimTest {
  @Test
  public void testBasic() {
    var analogInput = new AnalogInput(0);
    var analogEncoder = new AnalogEncoder(analogInput);
    var encoderSim = new AnalogEncoderSim(analogEncoder);

    encoderSim.setPosition(Rotation2d.fromDegrees(180));
    assertEquals(analogEncoder.get(), 0.5, 1E-8);
    assertEquals(encoderSim.getTurns(), 0.5, 1E-8);
    assertEquals(encoderSim.getPosition().getRadians(), Math.PI, 1E-8);
  }
}
