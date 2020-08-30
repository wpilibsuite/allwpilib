/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.XboxControllerSim;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

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
