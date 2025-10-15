// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

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

  @Test
  void testGetZ() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setZ(0.25);
    joysim.notifyNewData();
    assertEquals(0.25, joy.getZ(), 0.001);

    joysim.setZ(0);
    joysim.notifyNewData();
    assertEquals(0.0, joy.getZ(), 0.001);
  }

  @Test
  void testGetTwist() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setTwist(0.25);
    joysim.notifyNewData();
    assertEquals(0.25, joy.getTwist(), 0.001);

    joysim.setTwist(0);
    joysim.notifyNewData();
    assertEquals(0.0, joy.getTwist(), 0.001);
  }

  @Test
  void testGetThrottle() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setThrottle(0.25);
    joysim.notifyNewData();
    assertEquals(0.25, joy.getThrottle(), 0.001);

    joysim.setThrottle(0);
    joysim.notifyNewData();
    assertEquals(0.0, joy.getThrottle(), 0.001);
  }

  @Test
  void testGetTrigger() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setTrigger(true);
    joysim.notifyNewData();
    assertTrue(joy.getTrigger());

    joysim.setTrigger(false);
    joysim.notifyNewData();
    assertFalse(joy.getTrigger());
  }

  @Test
  void testGetTop() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    joysim.setTop(true);
    joysim.notifyNewData();
    assertTrue(joy.getTop());

    joysim.setTop(false);
    joysim.notifyNewData();
    assertFalse(joy.getTop());
  }

  @Test
  void testGetMagnitude() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    // X Only
    joysim.setX(0.5);
    joysim.setY(0.0);
    joysim.notifyNewData();
    assertEquals(0.5, joy.getMagnitude(), 0.001);

    // Y Only
    joysim.setX(0.0);
    joysim.setY(-0.5);
    joysim.notifyNewData();
    assertEquals(0.5, joy.getMagnitude(), 0.001);

    // Both
    joysim.setX(0.5);
    joysim.setY(-0.5);
    joysim.notifyNewData();
    assertEquals(0.70710678118, joy.getMagnitude(), 0.001);
  }

  @Test
  void testGetDirection() {
    HAL.initialize(500, 0);
    Joystick joy = new Joystick(1);
    JoystickSim joysim = new JoystickSim(joy);

    // X Only
    joysim.setX(0.5);
    joysim.setY(0.0);
    joysim.notifyNewData();
    assertEquals(90, joy.getDirectionDegrees(), 0.001);
    assertEquals(Math.toRadians(90), joy.getDirectionRadians(), 0.001);

    // Y Only
    joysim.setX(0.0);
    joysim.setY(-0.5);
    joysim.notifyNewData();
    assertEquals(0, joy.getDirectionDegrees(), 0.001);
    assertEquals(Math.toRadians(0), joy.getDirectionRadians(), 0.001);

    // Both
    joysim.setX(0.5);
    joysim.setY(-0.5);
    joysim.notifyNewData();
    assertEquals(45, joy.getDirectionDegrees(), 0.001);
    assertEquals(Math.toRadians(45), joy.getDirectionRadians(), 0.001);
  }
}
