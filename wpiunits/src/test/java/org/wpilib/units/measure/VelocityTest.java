// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.units.measure;

import static org.wpilib.units.Units.Milliseconds;
import static org.wpilib.units.Units.Seconds;
import static org.wpilib.units.Units.Volts;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;

import org.wpilib.units.Measure;
import org.wpilib.units.VelocityUnit;
import org.wpilib.units.VoltageUnit;
import org.junit.jupiter.api.Test;

class VelocityTest {
  @Test
  void velocityTimesTimeReturnsDivisor() {
    var velocity = VelocityUnit.combine(Volts, Seconds).of(123);
    var time = Milliseconds.of(100);
    Measure<VoltageUnit> result = velocity.times(time);
    // Compile-time test - would fail to compile if the return type was Mult or Measure<?>

    assertInstanceOf(Voltage.class, result);
  }
}
