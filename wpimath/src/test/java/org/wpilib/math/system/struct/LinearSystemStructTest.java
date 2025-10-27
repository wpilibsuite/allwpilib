// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N4;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class LinearSystemStructTest extends StructTestBase<LinearSystem<N2, N3, N4>> {
  LinearSystemStructTest() {
    super(
        new LinearSystem<>(
            MatBuilder.fill(Nat.N2(), Nat.N2(), 1.1, 1.2, 1.3, 1.4),
            MatBuilder.fill(Nat.N2(), Nat.N3(), 2.1, 2.2, 2.3, 2.4, 2.5, 2.6),
            MatBuilder.fill(Nat.N4(), Nat.N2(), 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8),
            MatBuilder.fill(
                Nat.N4(), Nat.N3(), 4.01, 4.02, 4.03, 4.04, 4.05, 4.06, 4.07, 4.08, 4.09, 4.10,
                4.11, 4.12)),
        LinearSystem.getStruct(Nat.N2(), Nat.N3(), Nat.N4()));
  }

  @Override
  public void checkEquals(LinearSystem<N2, N3, N4> testData, LinearSystem<N2, N3, N4> data) {
    assertEquals(testData.getA(), data.getA());
    assertEquals(testData.getB(), data.getB());
    assertEquals(testData.getC(), data.getC());
    assertEquals(testData.getD(), data.getD());
  }
}
