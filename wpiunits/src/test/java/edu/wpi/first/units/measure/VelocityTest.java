// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.units.Units.Volts;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.VelocityUnit;
import edu.wpi.first.units.VoltageUnit;
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
