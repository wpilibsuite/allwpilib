// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import edu.wpi.first.units.Units;
import edu.wpi.first.units.measure.Distance;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class InterpolatingMeasuresTreeMapTest {
  @Test
  void testInterpolationDistance() {
    InterpolatingTreeMap<Distance, Distance> table = new InterpolatingTreeMap<>(
        InverseInterpolator.forMeasure(),
        Interpolator.forMeasure());

    table.put(Units.Meters.of(125.0), Units.Meters.of(450.0));
    table.put(Units.Meters.of(200.0), Units.Meters.of(510.0));
    table.put(Units.Meters.of(268.0), Units.Meters.of(525.0));
    table.put(Units.Meters.of(312.0), Units.Meters.of(550.0));
    table.put(Units.Meters.of(326.0), Units.Meters.of(650.0));

    // Key below minimum gives the smallest value
    assertEquals(Units.Meters.of(450.0), table.get(Units.Meters.of(100.0)));

    // Minimum key gives exact value
    assertEquals(Units.Meters.of(450.0), table.get(Units.Meters.of(125.0)));

    // Key gives interpolated value
    assertEquals(Units.Meters.of(480.0), table.get(Units.Meters.of(162.5)));

    // Key at right of interpolation range gives exact value
    assertEquals(Units.Meters.of(510.0), table.get(Units.Meters.of(200.0)));

    // Maximum key gives exact value
    assertEquals(Units.Meters.of(650.0), table.get(Units.Meters.of(326.0)));

    // Key above maximum gives largest value
    assertEquals(Units.Meters.of(650.0), table.get(Units.Meters.of(400.0)));
  }

  @Test
  void testInterpolationClear() {
    InterpolatingTreeMap<Distance, Distance> table = new InterpolatingTreeMap<>(
        InverseInterpolator.forMeasure(),
        Interpolator.forMeasure());

    table.put(Units.Meters.of(125.0), Units.Meters.of(450.0));
    table.put(Units.Meters.of(200.0), Units.Meters.of(510.0));
    table.put(Units.Meters.of(268.0), Units.Meters.of(525.0));
    table.put(Units.Meters.of(312.0), Units.Meters.of(550.0));
    table.put(Units.Meters.of(326.0), Units.Meters.of(650.0));

    // Key below minimum gives the smallest value
    assertEquals(Units.Meters.of(450.0), table.get(Units.Meters.of(100.0)));

    // Minimum key gives exact value
    assertEquals(Units.Meters.of(450.0), table.get(Units.Meters.of(125.0)));

    // Key gives interpolated value
    assertEquals(Units.Meters.of(480.0), table.get(Units.Meters.of(162.5)));

    // Key at right of interpolation range gives exact value
    assertEquals(Units.Meters.of(510.0), table.get(Units.Meters.of(200.0)));

    // Maximum key gives exact value
    assertEquals(Units.Meters.of(650.0), table.get(Units.Meters.of(326.0)));

    // Key above maximum gives largest value
    assertEquals(Units.Meters.of(650.0), table.get(Units.Meters.of(400.0)));

    table.clear();

    table.put(Units.Meters.of(100.0), Units.Meters.of(250.0));
    table.put(Units.Meters.of(200.0), Units.Meters.of(500.0));

    assertEquals(Units.Meters.of(375.0), table.get(Units.Meters.of(150.0)));
  }
}
