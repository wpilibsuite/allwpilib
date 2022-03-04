// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class InterpolatingTreeMapTest {
  @Test
  void testInterpolation() {
    InterpolatingTreeMap<InterpolatingDouble, InterpolatingDouble> interpolatingTreeMap =
        new InterpolatingTreeMap<>();

    interpolatingTreeMap.put(new InterpolatingDouble(125.0), new InterpolatingDouble(450.0));
    interpolatingTreeMap.put(new InterpolatingDouble(200.0), new InterpolatingDouble(510.0));
    interpolatingTreeMap.put(new InterpolatingDouble(268.0), new InterpolatingDouble(525.0));
    interpolatingTreeMap.put(new InterpolatingDouble(312.0), new InterpolatingDouble(550.0));
    interpolatingTreeMap.put(new InterpolatingDouble(326.0), new InterpolatingDouble(650.0));

    assertEquals(
        new InterpolatingDouble(450.0).value,
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(100.0)).value);
    assertEquals(
        new InterpolatingDouble(450.0).value,
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(125.0)).value);
    assertEquals(
        new InterpolatingDouble(480.0).value,
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(162.5)).value);
    assertEquals(
        new InterpolatingDouble(510.0).value,
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(200.0)).value);
    assertEquals(
        new InterpolatingDouble(650.0).value,
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(326.0)).value);
    assertEquals(
        new InterpolatingDouble(650.0).value,
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(400.0)).value);
  }
}
