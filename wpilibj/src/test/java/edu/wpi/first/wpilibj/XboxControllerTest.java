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
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class XboxControllerTest {
  @ParameterizedTest
  @EnumSource(value = XboxController.Button.class)
  void testButtons(XboxController.Button button)
      throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    var buttonName = button.toString();

    String simSetMethodName = "set" + buttonName;
    String joyGetMethodName = "get" + buttonName;
    String joyPressedMethodName = "get" + buttonName + "Pressed";
    String joyReleasedMethodName = "get" + buttonName + "Released";

    final Method simSetMethod = joysim.getClass().getMethod(simSetMethodName, boolean.class);
    final Method joyGetMethod = joy.getClass().getMethod(joyGetMethodName);
    final Method joyPressedMethod = joy.getClass().getMethod(joyPressedMethodName);
    final Method joyReleasedMethod = joy.getClass().getMethod(joyReleasedMethodName);

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

  @ParameterizedTest
  @EnumSource(value = XboxController.Axis.class)
  void testAxes(XboxController.Axis axis)
      throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
    HAL.initialize(500, 0);
    XboxController joy = new XboxController(2);
    XboxControllerSim joysim = new XboxControllerSim(joy);

    var axisName = axis.toString();

    String simSetMethodName = "set" + axisName;
    String joyGetMethodName = "get" + axisName;

    Method simSetMethod = joysim.getClass().getMethod(simSetMethodName, double.class);
    Method joyGetMethod = joy.getClass().getMethod(joyGetMethodName);

    simSetMethod.invoke(joysim, 0.35);
    joysim.notifyNewData();
    assertEquals(0.35, (Double) joyGetMethod.invoke(joy), 0.001);
  }
}
