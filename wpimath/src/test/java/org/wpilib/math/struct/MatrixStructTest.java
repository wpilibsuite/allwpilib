// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.MatBuilder;
import org.wpilib.math.Matrix;
import org.wpilib.math.Nat;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N3;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class MatrixStructTest extends StructTestBase<Matrix<N2, N3>> {
  MatrixStructTest() {
    super(
        MatBuilder.fill(Nat.N2(), Nat.N3(), 1.1, 1.2, 1.3, 1.4, 1.5, 1.6),
        Matrix.getStruct(Nat.N2(), Nat.N3()));
  }

  @Override
  public void checkEquals(Matrix<N2, N3> testData, Matrix<N2, N3> data) {
    assertEquals(testData, data);
  }
}
