// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.math.linalg.DARE;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N5;
import org.wpilib.math.system.Discretization;
import org.wpilib.math.system.Models;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.StateSpaceUtil;

public final class DAREBenchmark {
  private static final Matrix<N5, N5> discA;
  private static final Matrix<N5, N2> discB;
  private static final Matrix<N5, N5> Q =
      StateSpaceUtil.costMatrix(VecBuilder.fill(0.0625, 0.125, 2.5, 0.95, 0.95));
  private static final Matrix<N2, N2> R = StateSpaceUtil.costMatrix(VecBuilder.fill(12.0, 12.0));

  static {
    //     [x ]
    //     [y ]       [Vₗ]
    // x = [θ ]   u = [Vᵣ]
    //     [vₗ]
    //     [vᵣ]
    var plant = Models.differentialDriveFromSysId(3.02, 0.642, 1.382, 0.08495);
    final double trackwidth = 0.9; // m
    final double velocity = 1.0; // m/s
    final double dt = 0.02; // s

    var plantA = plant.getA();
    var plantB = plant.getB();

    // spotless:off
    var A = MatBuilder.fill(Nat.N5(), Nat.N5(),
        0.0, 0.0, 0.0, 0.5, 0.5,
        0.0, 0.0, velocity, 0.0, 0.0,
        0.0, 0.0, 0.0, -1.0 / trackwidth, 1.0 / trackwidth,
        0.0, 0.0, 0.0, plantA.get(0, 0), plantA.get(0, 1),
        0.0, 0.0, 0.0, plantA.get(1, 0), plantA.get(1, 1));
    var B = MatBuilder.fill(Nat.N5(), Nat.N2(),
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0,
        plantB.get(0, 0), plantB.get(0, 1),
        plantB.get(1, 0), plantB.get(1, 1));
    // spotless:on

    var discABPair = Discretization.discretizeAB(A, B, dt);
    discA = discABPair.getFirst();
    discB = discABPair.getSecond();
  }

  private DAREBenchmark() {
    // Utility class.
  }

  /**
   * DARE benchmark.
   *
   * @return Solution to the DARE.
   */
  public static Matrix<N5, N5> dare() {
    return DARE.dareNoPrecond(discA, discB, Q, R);
  }
}
