// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ControlAffinePlantInversionFeedforwardTest {
  @SuppressWarnings("LocalVariableName")
  @Test
  void testCalculate() {
    ControlAffinePlantInversionFeedforward<N2, N1> feedforward =
            new ControlAffinePlantInversionFeedforward<N2, N1>(
                    Nat.N2(),
                    Nat.N1(),
                    this::getDynamics,
                    0.02);

    assertEquals(48.0, feedforward.calculate(
         VecBuilder.fill(2, 2),
         VecBuilder.fill(3, 3)).get(0, 0),
         1e-6);
  }

  @SuppressWarnings("LocalVariableName")
  @Test
  void testCalculateState() {
    ControlAffinePlantInversionFeedforward<N2, N1> feedforward =
        new ControlAffinePlantInversionFeedforward<N2, N1>(
            Nat.N2(),
            Nat.N1(),
            this::getDynamics,
            0.02);

    assertEquals(48.0, feedforward.calculate(
            VecBuilder.fill(2, 2),
            VecBuilder.fill(3, 3)).get(0, 0),
            1e-6);
  }

  @SuppressWarnings("ParameterName")
  protected Matrix<N2, N1> getDynamics(Matrix<N2, N1> x, Matrix<N1, N1> u) {
    var result = new Matrix<>(Nat.N2(), Nat.N1());

    result = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.000, 0, 0, 1.000).times(x)
            .plus(VecBuilder.fill(0, 1).times(u));

    return result;
  }

  @SuppressWarnings("ParameterName")
  protected Matrix<N2, N1> getStateDynamics(Matrix<N2, N1> x) {
    var result = new Matrix<>(Nat.N2(), Nat.N1());

    result = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.000, 0, 0, 1.000).times(x);

    return result;
  }
}
