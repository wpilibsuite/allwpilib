// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class DifferentialDriveAccelerationLimiterTest {
  @Test
  @SuppressWarnings("LocalVariableName")
  void testLowLimits() {
    final double trackwidth = 0.9;
    final double dt = 0.005;
    final double maxA = 2.0;
    final double maxAlpha = 2.0;

    var plant = LinearSystemId.identifyDrivetrainSystem(1.0, 1.0, 1.0, 1.0);

    var accelLimiter = new DifferentialDriveAccelerationLimiter(plant, trackwidth, maxA, maxAlpha);

    var x = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);
    var xAccelLimiter = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);

    // Ensure voltage exceeds acceleration before limiting
    {
      final var accels =
          plant
              .getA()
              .times(xAccelLimiter)
              .plus(plant.getB().times(new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0)));
      final double a = (accels.get(0, 0) + accels.get(1, 0)) / 2.0;
      assertTrue(Math.abs(a) > maxA);
    }
    {
      final var accels =
          plant
              .getA()
              .times(xAccelLimiter)
              .plus(plant.getB().times(new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, 12.0)));
      final double alpha = (accels.get(1, 0) - accels.get(0, 0)) / trackwidth;
      assertTrue(Math.abs(alpha) > maxAlpha);
    }

    // Forward
    var u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      final var voltages =
          accelLimiter.calculate(
              xAccelLimiter.get(0, 0), xAccelLimiter.get(1, 0), u.get(0, 0), u.get(1, 0));
      xAccelLimiter =
          plant.calculateX(
              xAccelLimiter,
              new MatBuilder<>(Nat.N2(), Nat.N1()).fill(voltages.left, voltages.right),
              dt);

      final var accels =
          plant
              .getA()
              .times(xAccelLimiter)
              .plus(
                  plant
                      .getB()
                      .times(
                          new MatBuilder<>(Nat.N2(), Nat.N1())
                              .fill(voltages.left, voltages.right)));
      final double a = (accels.get(0, 0) + accels.get(1, 0)) / 2.0;
      final double alpha = (accels.get(1, 0) - accels.get(0, 0)) / trackwidth;
      assertTrue(Math.abs(a) <= maxA);
      assertTrue(Math.abs(alpha) <= maxAlpha);
    }

    // Backward
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, -12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      final var voltages =
          accelLimiter.calculate(
              xAccelLimiter.get(0, 0), xAccelLimiter.get(1, 0), u.get(0, 0), u.get(1, 0));
      xAccelLimiter =
          plant.calculateX(
              xAccelLimiter,
              new MatBuilder<>(Nat.N2(), Nat.N1()).fill(voltages.left, voltages.right),
              dt);

      final var accels =
          plant
              .getA()
              .times(xAccelLimiter)
              .plus(
                  plant
                      .getB()
                      .times(
                          new MatBuilder<>(Nat.N2(), Nat.N1())
                              .fill(voltages.left, voltages.right)));
      final double a = (accels.get(0, 0) + accels.get(1, 0)) / 2.0;
      final double alpha = (accels.get(1, 0) - accels.get(0, 0)) / trackwidth;
      assertTrue(Math.abs(a) <= maxA);
      assertTrue(Math.abs(alpha) <= maxAlpha);
    }

    // Rotate CCW
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      final var voltages =
          accelLimiter.calculate(
              xAccelLimiter.get(0, 0), xAccelLimiter.get(1, 0), u.get(0, 0), u.get(1, 0));
      xAccelLimiter =
          plant.calculateX(
              xAccelLimiter,
              new MatBuilder<>(Nat.N2(), Nat.N1()).fill(voltages.left, voltages.right),
              dt);

      final var accels =
          plant
              .getA()
              .times(xAccelLimiter)
              .plus(
                  plant
                      .getB()
                      .times(
                          new MatBuilder<>(Nat.N2(), Nat.N1())
                              .fill(voltages.left, voltages.right)));
      final double a = (accels.get(0, 0) + accels.get(1, 0)) / 2.0;
      final double alpha = (accels.get(1, 0) - accels.get(0, 0)) / trackwidth;
      assertTrue(Math.abs(a) <= maxA);
      assertTrue(Math.abs(alpha) <= maxAlpha);
    }
  }

  @Test
  @SuppressWarnings("LocalVariableName")
  void testHighLimits() {
    final double trackwidth = 0.9;
    final double dt = 0.005;

    var plant = LinearSystemId.identifyDrivetrainSystem(1.0, 1.0, 1.0, 1.0);

    // Limits are so high, they don't get hit, so states of constrained and
    // unconstrained systems should match
    var accelLimiter = new DifferentialDriveAccelerationLimiter(plant, trackwidth, 1e3, 1e3);

    var x = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);
    var xAccelLimiter = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);

    // Forward
    var u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      final var voltages =
          accelLimiter.calculate(
              xAccelLimiter.get(0, 0), xAccelLimiter.get(1, 0), u.get(0, 0), u.get(1, 0));
      xAccelLimiter =
          plant.calculateX(
              xAccelLimiter,
              new MatBuilder<>(Nat.N2(), Nat.N1()).fill(voltages.left, voltages.right),
              dt);

      assertEquals(x.get(0, 0), xAccelLimiter.get(0, 0), 1e-5);
      assertEquals(x.get(1, 0), xAccelLimiter.get(1, 0), 1e-5);
    }

    // Backward
    x.fill(0.0);
    xAccelLimiter.fill(0.0);
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, -12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      final var voltages =
          accelLimiter.calculate(
              xAccelLimiter.get(0, 0), xAccelLimiter.get(1, 0), u.get(0, 0), u.get(1, 0));
      xAccelLimiter =
          plant.calculateX(
              xAccelLimiter,
              new MatBuilder<>(Nat.N2(), Nat.N1()).fill(voltages.left, voltages.right),
              dt);

      assertEquals(x.get(0, 0), xAccelLimiter.get(0, 0), 1e-5);
      assertEquals(x.get(1, 0), xAccelLimiter.get(1, 0), 1e-5);
    }

    // Rotate CCW
    x.fill(0.0);
    xAccelLimiter.fill(0.0);
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      final var voltages =
          accelLimiter.calculate(
              xAccelLimiter.get(0, 0), xAccelLimiter.get(1, 0), u.get(0, 0), u.get(1, 0));
      xAccelLimiter =
          plant.calculateX(
              xAccelLimiter,
              new MatBuilder<>(Nat.N2(), Nat.N1()).fill(voltages.left, voltages.right),
              dt);

      assertEquals(x.get(0, 0), xAccelLimiter.get(0, 0), 1e-5);
      assertEquals(x.get(1, 0), xAccelLimiter.get(1, 0), 1e-5);
    }
  }
}
