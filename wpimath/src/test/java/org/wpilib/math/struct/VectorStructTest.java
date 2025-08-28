// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.Nat;
import org.wpilib.math.VecBuilder;
import org.wpilib.math.Vector;
import org.wpilib.math.numbers.N2;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class VectorStructTest extends StructTestBase<Vector<N2>> {
  VectorStructTest() {
    super(VecBuilder.fill(1.1, 1.2), Vector.getStruct(Nat.N2()));
  }

  @Override
  public void checkEquals(Vector<N2> testData, Vector<N2> data) {
    assertEquals(testData, data);
  }
}
