// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.REVPHVersion;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
public class REVPHVersionStructTest extends StructTestBase<REVPHVersion> {
  public REVPHVersionStructTest() {
    super(new REVPHVersion(1, 2, 3, 4, 5, 999), REVPHVersion.struct);
  }

  @Override
  public void checkEquals(REVPHVersion testData, REVPHVersion data) {
    assertEquals(testData.firmwareMajor, data.firmwareMajor);
    assertEquals(testData.firmwareMinor, data.firmwareMinor);
    assertEquals(testData.firmwareFix, data.firmwareFix);
    assertEquals(testData.hardwareMinor, data.hardwareMinor);
    assertEquals(testData.hardwareMajor, data.hardwareMajor);
    assertEquals(testData.uniqueId, data.uniqueId);
  }
}
