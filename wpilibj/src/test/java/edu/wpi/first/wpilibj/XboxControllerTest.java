// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.XboxControllerSim;
import org.junit.jupiter.api.Test;

class XboxControllerTest {
  @Test
  void testGetX() {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    joysim.setX(XboxController.Hand.kLeft, 0.35);
    joysim.setX(XboxController.Hand.kRight, 0.45);
    joysim.notifyNewData();
    assertEquals(0.35, joy.getX(XboxController.Hand.kLeft), 0.001);
    assertEquals(0.45, joy.getX(XboxController.Hand.kRight), 0.001);
  }

  @Test
  void testGetBumper() {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    joysim.setBumper(XboxController.Hand.kLeft, false);
    joysim.setBumper(XboxController.Hand.kRight, true);
    joysim.notifyNewData();
    assertFalse(joy.getBumper(XboxController.Hand.kLeft));
    assertTrue(joy.getBumper(XboxController.Hand.kRight));
    // need to call pressed and released to clear flags
    joy.getBumperPressed(XboxController.Hand.kLeft);
    joy.getBumperReleased(XboxController.Hand.kLeft);
    joy.getBumperPressed(XboxController.Hand.kRight);
    joy.getBumperReleased(XboxController.Hand.kRight);

    joysim.setBumper(XboxController.Hand.kLeft, true);
    joysim.setBumper(XboxController.Hand.kRight, false);
    joysim.notifyNewData();
    assertTrue(joy.getBumper(XboxController.Hand.kLeft));
    assertTrue(joy.getBumperPressed(XboxController.Hand.kLeft));
    assertFalse(joy.getBumperReleased(XboxController.Hand.kLeft));
    assertFalse(joy.getBumper(XboxController.Hand.kRight));
    assertFalse(joy.getBumperPressed(XboxController.Hand.kRight));
    assertTrue(joy.getBumperReleased(XboxController.Hand.kRight));
  }

  @Test
  void testGetAButton() {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    joysim.setAButton(false);
    joysim.notifyNewData();
    assertFalse(joy.getAButton());
    // need to call pressed and released to clear flags
    joy.getAButtonPressed();
    joy.getAButtonReleased();

    joysim.setAButton(true);
    joysim.notifyNewData();
    assertTrue(joy.getAButton());
    assertTrue(joy.getAButtonPressed());
    assertFalse(joy.getAButtonReleased());

    joysim.setAButton(false);
    joysim.notifyNewData();
    assertFalse(joy.getAButton());
    assertFalse(joy.getAButtonPressed());
    assertTrue(joy.getAButtonReleased());
  }
}
