/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.VecBuilder;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class SingleJointedArmSimTest {
  SingleJointedArmSim m_sim = new SingleJointedArmSim(
      DCMotor.getVex775Pro(2),
      100,
      3.0,
      Units.inchesToMeters(19.0 / 2.0),
      -Math.PI,
      0.0, 10.0 / 2.2, true);

  @Test
  public void testArmDisabled() {
    m_sim.setState(VecBuilder.fill(0.0, 0.0));

    for (int i = 0; i < 12 / 0.02; i++) {
      m_sim.setInput(0.0);
      m_sim.update(0.020);
    }

    // the arm should swing down
    assertEquals(-Math.PI / 2.0, m_sim.getAngleRads(), 0.1);
  }
}
