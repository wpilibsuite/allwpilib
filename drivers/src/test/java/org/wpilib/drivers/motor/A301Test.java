// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.motor;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.wpilib.hardware.bus.CANBusMap;
import org.wpilib.hardware.hal.HAL;

class A301Test {
  @BeforeAll
  static void initializeHal() {
    HAL.initialize();
  }

  @Test
  void controlsA301ThroughHighLevelApi() {
    A301 a301 = new A301(CANBusMap.CAN_S0, 31);
    assertEquals(0, a301.getBusId());
    assertEquals(31, a301.getDeviceId());

    a301.setThrottle(0.25);
    assertEquals(0.25, a301.getThrottle());
    a301.setVoltage(6.0);
    assertEquals(0.5, a301.getThrottle());

    assertEquals(A301Error.kOk, a301.setVelocity(125.0));
    assertEquals(A301Error.kOk, a301.setRelativePosition(3.0));
    assertEquals(A301Error.kOk, a301.setRelativePositionWithSpeed(3.0, 50.0));
    assertEquals(A301Error.kOk, a301.setAbsolutePosition(0.25));
    assertEquals(A301Error.kOk, a301.setAbsolutePositionWithSpeed(0.25, 50.0));
    assertEquals(A301Error.kOk, a301.setCurrent(10.0));
    assertEquals(A301Error.kOk, a301.setRelativeEncoderPosition(2.0));
    assertEquals(A301Error.kOk, a301.setAbsoluteEncoderPosition(0.1));
    assertEquals(A301Error.kOk, a301.setIdleMode(A301.IdleMode.BRAKE));
    assertEquals(A301Error.kOk, a301.enableAbsolutePositionContinuousInput());
    assertEquals(A301Error.kOk, a301.disableAbsolutePositionContinuousInput());
    assertEquals(A301Error.kOk, a301.clearFaults());

    a301.setInverted(true);
    assertTrue(a301.getInverted());
    assertEquals(A301Error.kOk, a301.setAbsoluteEncoderRangeOffset(0.25));
    assertEquals(A301Error.kParamInvalid, a301.setAbsoluteEncoderRangeOffset(0.75));

    A301StatusSignal<Double> voltage = a301.getBusVoltage();
    assertFalse(voltage.isValid());
    assertEquals(A301Error.kTimeout, voltage.getError());

    a301.disable();
    assertEquals(0.0, a301.getThrottle());
    a301.close();
    a301.close();
    assertThrows(IllegalStateException.class, a301::getThrottle);
  }
}
