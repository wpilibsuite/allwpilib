// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.util.Nat;

class ControlAffinePlantInversionFeedforwardTest {
  @Test
  void testCalculate() {
    ControlAffinePlantInversionFeedforward<N2, N1> feedforward =
        new ControlAffinePlantInversionFeedforward<>(Nat.N2(), Nat.N1(), this::getDynamics, 0.02);

    assertEquals(
        48.0, feedforward.calculate(VecBuilder.fill(2, 2), VecBuilder.fill(3, 3)).get(0, 0), 1e-6);
  }

  @Test
  void testCalculateState() {
    ControlAffinePlantInversionFeedforward<N2, N1> feedforward =
        new ControlAffinePlantInversionFeedforward<>(Nat.N2(), Nat.N1(), this::getDynamics, 0.02);

    assertEquals(
        48.0, feedforward.calculate(VecBuilder.fill(2, 2), VecBuilder.fill(3, 3)).get(0, 0), 1e-6);
  }

  @Test
  void testCalculateStateDependentB() {
    var feedforward =
        new ControlAffinePlantInversionFeedforward<>(
            Nat.N2(), Nat.N1(), this::getStateDependentInputDynamics, 0.02);

    // f(x, u) = x + B(x)u, where B(x) = [0, 1 + x₀²]ᵀ
    //
    // ṙ = (rₖ₊₁ − rₖ)/dt
    // ṙ = f(rₖ, 0) + B(rₖ)u
    // u = B(rₖ)⁺(ṙ − f(rₖ, 0))
    //   = B(rₖ)⁺(ṙ − rₖ)
    //   = B(rₖ)ᵀ(ṙ − rₖ) / (B(rₖ)ᵀB(rₖ))
    //
    // rₖ = [2, 2]ᵀ, rₖ₊₁ = [3, 3]ᵀ, dt = 0.02
    // B(rₖ) = [0, 1 + 2²]ᵀ = [0, 5]ᵀ
    // ṙ = ([3, 3]ᵀ − [2, 2]ᵀ)/0.02 = [50, 50]ᵀ
    // ṙ − rₖ = [50, 50]ᵀ − [2, 2]ᵀ = [48, 48]ᵀ
    // u = (0 ⋅ 48 + 5 ⋅ 48) / (0² + 5²) = 240/25 = 9.6
    assertEquals(
        9.6, feedforward.calculate(VecBuilder.fill(2, 2), VecBuilder.fill(3, 3)).get(0, 0), 1e-6);

    // rₖ = [3, 3]ᵀ, rₖ₊₁ = [4, 4]ᵀ, dt = 0.02
    // B(rₖ) = [0, 1 + 3²]ᵀ = [0, 10]ᵀ
    // ṙ = ([4, 4]ᵀ − [3, 3]ᵀ)/0.02 = [50, 50]ᵀ
    // ṙ − rₖ = [50, 50]ᵀ − [3, 3]ᵀ = [47, 47]ᵀ
    // u = (0 ⋅ 47 + 10 ⋅ 47) / (0² + 10²) = 470/100 = 4.7
    assertEquals(
        4.7, feedforward.calculate(VecBuilder.fill(3, 3), VecBuilder.fill(4, 4)).get(0, 0), 1e-6);
  }

  protected Matrix<N2, N1> getDynamics(Matrix<N2, N1> x, Matrix<N1, N1> u) {
    return MatBuilder.fill(Nat.N2(), Nat.N2(), 1.000, 0, 0, 1.000)
        .times(x)
        .plus(VecBuilder.fill(0, 1).times(u));
  }

  protected Matrix<N2, N1> getStateDynamics(Matrix<N2, N1> x) {
    return MatBuilder.fill(Nat.N2(), Nat.N2(), 1.000, 0, 0, 1.000).times(x);
  }

  protected Matrix<N2, N1> getStateDependentInputDynamics(Matrix<N2, N1> x, Matrix<N1, N1> u) {
    return MatBuilder.fill(Nat.N2(), Nat.N2(), 1.000, 0, 0, 1.000)
        .times(x)
        .plus(VecBuilder.fill(0, 1 + x.get(0, 0) * x.get(0, 0)).times(u));
  }
}
