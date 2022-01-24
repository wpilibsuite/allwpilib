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
  SingleJointedArmSim m_sim =
      new SingleJointedArmSim(
          DCMotor.getVex775Pro(2),
          100,
          3.0,
          Units.inchesToMeters(19.0 / 2.0),
          -Math.PI,
          0.0,
          10.0 / 2.2,
          true);

  @Test
  void testArmDisabled() {
    m_sim.setState(VecBuilder.fill(0.0, 0.0));

    for (int i = 0; i < 12 / 0.02; i++) {
      m_sim.setInput(0.0);
      m_sim.update(0.020);
    }

    // the arm should swing down
    assertEquals(-Math.PI / 2.0, m_sim.getAngleRads(), 0.1);
  }
}
