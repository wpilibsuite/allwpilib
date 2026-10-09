// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N7;
import org.wpilib.math.util.Nat;

public final class MatrixExpBenchmark {
  private MatrixExpBenchmark() {
    // Utility class.
  }

  /** Matrix exponential benchmark. */
  public static Matrix<N7, N7> matrixExp() {
    // Pascal matrix
    //
    //    ([0  0  0  0  0  0  0])   [1  0   0   0   0  0  0]
    //    ([1  0  0  0  0  0  0])   [1  1   0   0   0  0  0]
    //    ([0  2  0  0  0  0  0])   [1  2   1   0   0  0  0]
    // exp([0  0  3  0  0  0  0]) = [1  3   3   1   0  0  0]
    //    ([0  0  0  4  0  0  0])   [1  4   6   4   1  0  0]
    //    ([0  0  0  0  5  0  0])   [1  5  10  10   5  1  0]
    //    ([0  0  0  0  0  6  0])   [1  6  15  20  15  6  1]
    var pascal = new Matrix<>(Nat.N7(), Nat.N7());
    for (int col = 0; col < 6; ++col) {
      pascal.set(col + 1, col, col + 1);
    }
    return pascal.exp();
  }
}
