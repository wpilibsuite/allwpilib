// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class ImplicitModelFollowerTest {
  @Test
  void testSameModel() {
    final double dt = 0.005;

    var plant = LinearSystemId.identifyDrivetrainSystem(1.0, 1.0, 1.0, 1.0);

    var imf = new ImplicitModelFollower<N2, N2, N2>(plant, plant);

    var x = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);
    var xImf = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);

    // Forward
    var u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0));
      assertEquals(x.get(1, 0), xImf.get(1, 0));
    }

    // Backward
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, -12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0));
      assertEquals(x.get(1, 0), xImf.get(1, 0));
    }

    // Rotate CCW
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0));
      assertEquals(x.get(1, 0), xImf.get(1, 0));
    }
  }

  @Test
  void testSlowerRefModel() {
    final double dt = 0.005;

    var plant = LinearSystemId.identifyDrivetrainSystem(1.0, 1.0, 1.0, 1.0);

    // Linear acceleration is slower, but angular acceleration is the same
    var plantRef = LinearSystemId.identifyDrivetrainSystem(1.0, 2.0, 1.0, 1.0);

    var imf = new ImplicitModelFollower<N2, N2, N2>(plant, plantRef);

    var x = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);
    var xImf = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.0, 0.0);

    // Forward
    var u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertTrue(x.get(0, 0) >= xImf.get(0, 0));
      assertTrue(x.get(1, 0) >= xImf.get(1, 0));
    }

    // Backward
    x.fill(0.0);
    xImf.fill(0.0);
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, -12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertTrue(x.get(0, 0) <= xImf.get(0, 0));
      assertTrue(x.get(1, 0) <= xImf.get(1, 0));
    }

    // Rotate CCW
    x.fill(0.0);
    xImf.fill(0.0);
    u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(-12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0), 1e-5);
      assertEquals(x.get(1, 0), xImf.get(1, 0), 1e-5);
    }
  }
}
