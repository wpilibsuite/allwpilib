// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.XboxControllerSim;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

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
  void testGetY() {
    HAL.initialize(500, 0);
    XboxControllerSim joysim = new XboxControllerSim(2);

    joysim.setY(XboxController.Hand.kLeft, 0.35);
    joysim.setY(XboxController.Hand.kRight, 0.45);
    joysim.notifyNewData();

    XboxController joy = new XboxController(2);
    assertEquals(0.35, joy.getY(XboxController.Hand.kLeft), 0.001);
    assertEquals(0.45, joy.getY(XboxController.Hand.kRight), 0.001);
  }

  @Test
  void testGetTrigger() {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    joysim.setTriggerAxis(XboxController.Hand.kLeft, 0.35);
    joysim.setTriggerAxis(XboxController.Hand.kRight, 0.45);
    joysim.notifyNewData();
    assertEquals(0.35, joy.getTriggerAxis(XboxController.Hand.kLeft), 0.001);
    assertEquals(0.45, joy.getTriggerAxis(XboxController.Hand.kRight), 0.001);
  }

  private static Stream<String> getStandardButtonsArguments() {
    return Stream.of("A", "B", "X", "Y", "Back", "Start");
  }

  @ParameterizedTest
  @MethodSource("getStandardButtonsArguments")
  @SuppressWarnings({"VariableDeclarationUsageDistance"})
  public void testStandardButtons(String buttonName)
      throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    String simSetMethodName = "set" + buttonName + "Button";
    String joyGetMethodName = "get" + buttonName + "Button";
    String joyPressedMethodName = "get" + buttonName + "ButtonPressed";
    String joyReleasedMethodName = "get" + buttonName + "ButtonReleased";

    Method simSetMethod = joysim.getClass().getMethod(simSetMethodName, boolean.class);
    Method joyGetMethod = joy.getClass().getMethod(joyGetMethodName);
    Method joyPressedMethod = joy.getClass().getMethod(joyPressedMethodName);
    Method joyReleasedMethod = joy.getClass().getMethod(joyReleasedMethodName);

    simSetMethod.invoke(joysim, false);
    joysim.notifyNewData();
    assertFalse((Boolean) joyGetMethod.invoke(joy));
    // need to call pressed and released to clear flags
    joyPressedMethod.invoke(joy);
    joyReleasedMethod.invoke(joy);

    simSetMethod.invoke(joysim, true);
    joysim.notifyNewData();
    assertTrue((Boolean) joyGetMethod.invoke(joy));
    assertTrue((Boolean) joyPressedMethod.invoke(joy));
    assertFalse((Boolean) joyReleasedMethod.invoke(joy));

    simSetMethod.invoke(joysim, false);
    joysim.notifyNewData();
    assertFalse((Boolean) joyGetMethod.invoke(joy));
    assertFalse((Boolean) joyPressedMethod.invoke(joy));
    assertTrue((Boolean) joyReleasedMethod.invoke(joy));
  }

  private static Stream<Arguments> getStickButtonsArguments() {
    return Stream.of(
        Arguments.of(GenericHID.Hand.kLeft, "Bumper"),
        Arguments.of(GenericHID.Hand.kRight, "Bumper"),
        Arguments.of(GenericHID.Hand.kLeft, "StickButton"),
        Arguments.of(GenericHID.Hand.kRight, "StickButton"));
  }

  @ParameterizedTest
  @MethodSource("getStickButtonsArguments")
  @SuppressWarnings({"VariableDeclarationUsageDistance"})
  public void testStickButtons(GenericHID.Hand hand, String buttonName)
      throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    String simSetMethodName = "set" + buttonName;
    String joyGetMethodName = "get" + buttonName;
    String joyPressedMethodName = "get" + buttonName + "Pressed";
    String joyReleasedMethodName = "get" + buttonName + "Released";

    Method simSetMethod =
        joysim.getClass().getMethod(simSetMethodName, GenericHID.Hand.class, boolean.class);
    Method joyGetMethod = joy.getClass().getMethod(joyGetMethodName, GenericHID.Hand.class);
    Method joyPressedMethod = joy.getClass().getMethod(joyPressedMethodName, GenericHID.Hand.class);
    Method joyReleasedMethod =
        joy.getClass().getMethod(joyReleasedMethodName, GenericHID.Hand.class);

    simSetMethod.invoke(joysim, hand, false);
    joysim.notifyNewData();
    assertFalse((Boolean) joyGetMethod.invoke(joy, hand));
    // need to call pressed and released to clear flags
    joyPressedMethod.invoke(joy, hand);
    joyReleasedMethod.invoke(joy, hand);

    simSetMethod.invoke(joysim, hand, true);
    joysim.notifyNewData();
    assertTrue((Boolean) joyGetMethod.invoke(joy, hand));
    assertTrue((Boolean) joyPressedMethod.invoke(joy, hand));
    assertFalse((Boolean) joyReleasedMethod.invoke(joy, hand));

    simSetMethod.invoke(joysim, hand, false);
    joysim.notifyNewData();
    assertFalse((Boolean) joyGetMethod.invoke(joy, hand));
    assertFalse((Boolean) joyPressedMethod.invoke(joy, hand));
    assertTrue((Boolean) joyReleasedMethod.invoke(joy, hand));
  }
}
