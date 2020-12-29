// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.JoystickSim;
import org.junit.jupiter.api.Test;

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
