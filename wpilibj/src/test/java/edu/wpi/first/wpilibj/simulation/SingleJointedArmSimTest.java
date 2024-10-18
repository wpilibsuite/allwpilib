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
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class SingleJointedArmSimTest {
  Gearbox m_gearbox = new Gearbox(2, DCMotor.Vex775Pro, 300.0);
  LinearSystem<N2, N1, N2> m_plant =
      LinearSystemId.createSingleJointedArmSystem(
          m_gearbox, 15.5, Units.inchesToMeters(30.0), Units.inchesToMeters(15.0));
  SingleJointedArmSim m_sim =
      new SingleJointedArmSim(
          m_plant,
          m_gearbox,
          Units.inchesToMeters(30.0),
          Units.inchesToMeters(15.0),
          -Math.PI,
          0.0,
          -9.8,
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
