// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.Nat;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.linalg.Vector;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.proto.Wpimath.ProtobufVector;
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
