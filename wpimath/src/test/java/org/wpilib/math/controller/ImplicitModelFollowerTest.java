// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.math.Matrix;
import org.wpilib.math.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class ImplicitModelFollowerTest {
  @Test
  void testSameModel() {
    final double dt = 0.005;

    var plant = LinearSystemId.identifyDrivetrainSystem(1.0, 1.0, 1.0, 1.0);

    var imf = new ImplicitModelFollower<>(plant, plant);

    Matrix<N2, N1> x = VecBuilder.fill(0.0, 0.0);
    Matrix<N2, N1> xImf = VecBuilder.fill(0.0, 0.0);

    // Forward
    var u = VecBuilder.fill(12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0));
      assertEquals(x.get(1, 0), xImf.get(1, 0));
    }

    // Backward
    u = VecBuilder.fill(-12.0, -12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0));
      assertEquals(x.get(1, 0), xImf.get(1, 0));
    }

    // Rotate CCW
    u = VecBuilder.fill(-12.0, 12.0);
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

    var imf = new ImplicitModelFollower<>(plant, plantRef);

    Matrix<N2, N1> x = VecBuilder.fill(0.0, 0.0);
    Matrix<N2, N1> xImf = VecBuilder.fill(0.0, 0.0);

    // Forward
    var u = VecBuilder.fill(12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertTrue(x.get(0, 0) >= xImf.get(0, 0));
      assertTrue(x.get(1, 0) >= xImf.get(1, 0));
    }

    // Backward
    x.fill(0.0);
    xImf.fill(0.0);
    u = VecBuilder.fill(-12.0, -12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertTrue(x.get(0, 0) <= xImf.get(0, 0));
      assertTrue(x.get(1, 0) <= xImf.get(1, 0));
    }

    // Rotate CCW
    x.fill(0.0);
    xImf.fill(0.0);
    u = VecBuilder.fill(-12.0, 12.0);
    for (double t = 0.0; t < 3.0; t += dt) {
      x = plant.calculateX(x, u, dt);
      xImf = plant.calculateX(xImf, imf.calculate(xImf, u), dt);

      assertEquals(x.get(0, 0), xImf.get(0, 0), 1e-5);
      assertEquals(x.get(1, 0), xImf.get(1, 0), 1e-5);
    }
  }
}
