// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Inches;
import static edu.wpi.first.units.Units.Revolutions;
import static edu.wpi.first.units.Units.Second;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class EncoderTest {
  static class Encoder<U extends Unit<U>> {
    int m_ticks; // = 0
    private Measure<U> m_distancePerPulse;
    private MutableMeasure<U> m_distance;
    private MutableMeasure<Velocity<U>> m_rate;

    void setDistancePerPulse(Measure<U> distancePerPulse) {
      m_distancePerPulse = distancePerPulse;
      m_distance = MutableMeasure.zero(distancePerPulse.unit());
      m_rate = MutableMeasure.zero(distancePerPulse.unit().per(Second));
    }

    Measure<U> getDistance() {
      return m_distance;
    }

    Measure<Velocity<U>> getRate() {
      return m_rate;
    }

    void setTicks(int ticks) {
      // pretend we read from JNI here instead of being passed a specific value
      var change = ticks - m_ticks;
      m_ticks = ticks;
      m_distance.mut_setMagnitude(m_distancePerPulse.magnitude() * ticks);

      // assumes the last update was 1 second ago - fine for tests
      m_rate.mut_setMagnitude(m_distancePerPulse.magnitude() * change);
    }
  }

  @Test
  void testAsDistance() {
    double ticksPerRevolution = 2048;

    var encoder = new Encoder<Distance>();

    // distance per rotation = (wheel circumference / gear ratio)
    // distance per tick = distance per rotation / ticks per rotation
    var wheelDiameter = Inches.of(6);
    double gearRatio = 10; // 10:1 ratio
    Measure<Distance> distancePerPulse =
        wheelDiameter.times(Math.PI).divide(gearRatio).divide(ticksPerRevolution);
    encoder.setDistancePerPulse(distancePerPulse);

    encoder.m_ticks = 0;
    assertEquals(0, encoder.getDistance().in(Inches), Measure.EQUIVALENCE_THRESHOLD);
    assertEquals(0, encoder.getRate().in(Inches.per(Second)), Measure.EQUIVALENCE_THRESHOLD);

    // one full encoder turn, 1/10th of a wheel rotation
    encoder.setTicks(2048);
    assertEquals(6 * Math.PI / 10, encoder.getDistance().in(Inches), Measure.EQUIVALENCE_THRESHOLD);

    // one full encoder turn back, 1/10th of a wheel rotation - rate should be negative
    encoder.setTicks(0);
    assertEquals(
        -6 * Math.PI / 10, encoder.getRate().in(Inches.per(Second)), Measure.EQUIVALENCE_THRESHOLD);
  }

  @Test
  void testAsRevolutions() {
    double ticksPerRevolution = 2048;

    var encoder = new Encoder<Angle>();

    Measure<Angle> distancePerPulse = Revolutions.of(1).divide(ticksPerRevolution);
    encoder.setDistancePerPulse(distancePerPulse);

    encoder.m_ticks = 0;
    assertEquals(0, encoder.getDistance().in(Revolutions), Measure.EQUIVALENCE_THRESHOLD);
    assertEquals(0, encoder.getRate().in(Revolutions.per(Second)), Measure.EQUIVALENCE_THRESHOLD);

    encoder.setTicks(2048); // one full encoder turn, 1/10th of a wheel rotation
    assertEquals(1, encoder.getDistance().in(Revolutions), Measure.EQUIVALENCE_THRESHOLD);
    assertEquals(1, encoder.getRate().in(Revolutions.per(Second)), Measure.EQUIVALENCE_THRESHOLD);
  }
}
