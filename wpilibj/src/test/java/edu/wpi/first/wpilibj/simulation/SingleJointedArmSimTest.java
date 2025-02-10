// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class SingleJointedArmSimTest {
  @Test
  void testArmDisabled() {
    SingleJointedArmSim sim =
        new SingleJointedArmSim(
            DCMotor.getVex775Pro(2),
            300,
            3.0,
            Units.inchesToMeters(30.0),
            -Math.PI,
            0.0,
            true,
            Math.PI / 2.0);

    // Reset Arm angle to 0
    sim.setState(VecBuilder.fill(0.0, 0.0));

    for (int i = 0; i < 12 / 0.02; i++) {
      sim.setInput(0.0);
      sim.update(0.020);
    }

    // the arm should swing down
    assertEquals(-Math.PI / 2.0, m_sim.getAngle(), 0.1);
  }
}
