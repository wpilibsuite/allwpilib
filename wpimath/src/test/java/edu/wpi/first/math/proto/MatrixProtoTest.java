// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.proto.Wpimath.ProtobufMatrix;
import edu.wpi.first.wpilibj.ProtoTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class MatrixProtoTest extends ProtoTestBase<Matrix<N2, N3>, ProtobufMatrix> {
  MatrixProtoTest() {
    super(
        MatBuilder.fill(Nat.N2(), Nat.N3(), 1.1, 1.2, 1.3, 1.4, 1.5, 1.6),
        Matrix.getProto(Nat.N2(), Nat.N3()));
  }

  @Override
  public void checkEquals(Matrix<N2, N3> testData, Matrix<N2, N3> data) {
    assertEquals(testData, data);
  }
}
