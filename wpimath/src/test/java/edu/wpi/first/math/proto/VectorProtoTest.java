// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.proto.Wpimath.ProtobufVector;
import edu.wpi.first.wpilibj.ProtoTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class VectorProtoTest extends ProtoTestBase<Vector<N2>, ProtobufVector> {
  VectorProtoTest() {
    super(VecBuilder.fill(1.1, 1.2), Vector.getProto(Nat.N2()));
  }

  @Override
  public void checkEquals(Vector<N2> testData, Vector<N2> data) {
    assertEquals(testData, data);
  }
}
