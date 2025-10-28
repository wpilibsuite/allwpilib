// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.function.DoubleUnaryOperator;
import org.junit.jupiter.api.Test;

class SimulatedAnnealingTest {
  @Test
  void testDoubleFunctionOptimizationHeartBeat() {
    DoubleUnaryOperator function = x -> -(x + Math.sin(x)) * Math.exp(-x * x) + 1;

    double stepSize = 10.0;

    var simulatedAnnealing =
        new SimulatedAnnealing<Double>(
            2.0,
            x -> Math.clamp(x + (Math.random() - 0.5) * stepSize, -3, 3),
            function::applyAsDouble);

    double solution = simulatedAnnealing.solve(-1.0, 5000);

    assertEquals(0.68, solution, 1e-1);
  }

  @Test
  void testDoubleFunctionOptimizationMultimodal() {
    DoubleUnaryOperator function = x -> Math.sin(x) + Math.sin((10.0 / 3.0) * x);

    double stepSize = 10.0;

    var simulatedAnnealing =
        new SimulatedAnnealing<Double>(
            2.0,
            x -> Math.clamp(x + (Math.random() - 0.5) * stepSize, 0, 7),
            function::applyAsDouble);

    double solution = simulatedAnnealing.solve(-1.0, 5000);

    assertEquals(5.146, solution, 1e-1);
  }
}
