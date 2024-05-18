// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class SingleJointedArmSimTest {
  double m_armMassKg = 15.500;
  double m_armLengthMeters = Units.inchesToMeters(30.0);
  LinearSystem<N2, N1, N2> m_plant =
      LinearSystemId.createSingleJointedArmSystem(
          DCMotor.getVex775Pro(2), m_armMassKg, m_armLengthMeters, 300);
  SingleJointedArmSim m_sim =
      new SingleJointedArmSim(
          m_plant,
          DCMotor.getVex775Pro(2),
          300,
          m_armLengthMeters,
          -Math.PI,
          0.0,
          true,
          Math.PI / 2.0);

  @Test
  void testArmDisabled() {
    // Reset Arm angle to 0
    m_sim.setState(VecBuilder.fill(0.0, 0.0));

    for (int i = 0; i < 12 / 0.02; i++) {
      m_sim.setInput(0.0);
      m_sim.update(0.020);
    }

    // the arm should swing down
    assertEquals(-Math.PI / 2.0, m_sim.getAngleRads(), 0.1);
  }
}
