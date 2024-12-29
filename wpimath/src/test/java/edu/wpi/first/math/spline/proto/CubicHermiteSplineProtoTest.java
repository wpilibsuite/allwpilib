// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline.proto;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;

import edu.wpi.first.math.proto.Spline.ProtobufCubicHermiteSpline;
import edu.wpi.first.math.spline.CubicHermiteSpline;
import edu.wpi.first.wpilibj.ProtoTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class CubicHermiteSplineProtoTest
    extends ProtoTestBase<CubicHermiteSpline, ProtobufCubicHermiteSpline> {
  CubicHermiteSplineProtoTest() {
    super(
        new CubicHermiteSpline(
            new double[] {0.1, 0.3},
            new double[] {0.4, -0.2},
            new double[] {1.5, 1.3},
            new double[] {-2.4, -1.1}),
        CubicHermiteSpline.proto);
  }

  @Override
  public void checkEquals(CubicHermiteSpline testData, CubicHermiteSpline data) {
    assertArrayEquals(testData.xInitialControlVector, data.xInitialControlVector);
    assertArrayEquals(testData.xFinalControlVector, data.xFinalControlVector);
    assertArrayEquals(testData.yInitialControlVector, data.yInitialControlVector);
    assertArrayEquals(testData.yFinalControlVector, data.yFinalControlVector);
  }
}
