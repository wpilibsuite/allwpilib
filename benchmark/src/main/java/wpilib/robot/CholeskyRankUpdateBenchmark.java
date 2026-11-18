// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N5;
import org.wpilib.math.util.Nat;

public final class CholeskyRankUpdateBenchmark {
  // spotless:off
  // Symmetric positive definite matrix
  private static final Matrix<N5, N5> A = MatBuilder.fill(Nat.N5(), Nat.N5(),
      4.0, 1.0, 0.5, 0.0, 0.2,
      1.0, 5.0, 1.0, 0.3, 0.0,
      0.5, 1.0, 6.0, 1.0, 0.4,
      0.0, 0.3, 1.0, 7.0, 1.0,
      0.2, 0.0, 0.4, 1.0, 8.0);
  // spotless:on
  private static final Matrix<N5, N5> L = A.lltDecompose(true);
  private static final Matrix<N5, N1> v = VecBuilder.fill(0.1, 0.2, 0.3, 0.4, 0.5);

  private CholeskyRankUpdateBenchmark() {
    // Utility class.
  }

  private static Matrix<N5, N5> rankUpdate(double sigma) {
    // The rank update is in-place, so start from the original factor each
    // iteration
    var S = L.copy();
    S.rankUpdate(v, sigma, true);
    return S;
  }

  /**
   * Cholesky rank update benchmark.
   *
   * @return Updated Cholesky factor.
   */
  public static Matrix<N5, N5> update() {
    return rankUpdate(1.0);
  }

  /**
   * Cholesky rank downdate benchmark.
   *
   * @return Downdated Cholesky factor.
   */
  public static Matrix<N5, N5> downdate() {
    return rankUpdate(-1.0);
  }
}
