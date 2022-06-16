// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.numbers.N1;
import java.util.function.DoubleFunction;
import org.junit.jupiter.api.Test;

class SimulatedAnnealingTest {
  @Test
  void testDoubleFunctionOptimizationHeartBeat() {
    DoubleFunction<Double> function = x -> -(x + Math.sin(x)) * Math.exp(-x * x) + 1;

    double stepSize = 10.0;

    SimulatedAnnealing<N1> simulatedAnnealing =
        new SimulatedAnnealing<>(
            5000,
            2.0,
            x ->
                VecBuilder.fill(
                    MathUtil.clamp(x.get(0, 0) + (Math.random() - 0.5) * stepSize, -3, 3)),
            x -> function.apply(x.get(0, 0)));

    Vector<N1> ans = simulatedAnnealing.minimize(VecBuilder.fill(-1));

    assertTrue(ans.isEqual(VecBuilder.fill(0.68), 1E-1));
  }

  @Test
  void testDoubleFunctionOptimizationMultimodal() {
    DoubleFunction<Double> function = x -> Math.sin(x) + Math.sin((10.0 / 3.0) * x);

    double stepSize = 10.0;

    SimulatedAnnealing<N1> simulatedAnnealing =
        new SimulatedAnnealing<>(
            5000,
            2.0,
            x ->
                VecBuilder.fill(
                    MathUtil.clamp(x.get(0, 0) + (Math.random() - 0.5) * stepSize, 0, 7)),
            x -> function.apply(x.get(0, 0)));

    Vector<N1> ans = simulatedAnnealing.minimize(VecBuilder.fill(-1));

    assertTrue(ans.isEqual(VecBuilder.fill(5.146), 1E-1));
  }
}
