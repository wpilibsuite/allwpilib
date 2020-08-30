/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.JoystickSim;

import static org.junit.jupiter.api.Assertions.assertEquals;

class JoystickTest {
  @Test
  void testGetX() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setX(0.25);
    joysim.notifyNewData();
    assertEquals(0.25, joy.getX(), 0.001);

    joysim.setX(0);
    joysim.notifyNewData();
    assertEquals(0.0, joy.getX(), 0.001);
  }

  @Test
  void testGetY() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setY(0.25);
    joysim.notifyNewData();
    assertEquals(0.25, joy.getY(), 0.001);

    joysim.setY(0);
    joysim.notifyNewData();
    assertEquals(0.0, joy.getY(), 0.001);
  }
}
