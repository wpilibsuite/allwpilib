// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.PowerDistributionVersion;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
public class PowerDistributionVersionStructTest extends StructTestBase<PowerDistributionVersion> {
  public PowerDistributionVersionStructTest() {
    super(new PowerDistributionVersion(1, 2, 3, 4, 5, 999), PowerDistributionVersion.struct);
  }

  @Override
  public void checkEquals(PowerDistributionVersion testData, PowerDistributionVersion data) {
    assertEquals(testData.firmwareMajor, data.firmwareMajor);
    assertEquals(testData.firmwareMinor, data.firmwareMinor);
    assertEquals(testData.firmwareFix, data.firmwareFix);
    assertEquals(testData.hardwareMinor, data.hardwareMinor);
    assertEquals(testData.hardwareMajor, data.hardwareMajor);
    assertEquals(testData.uniqueId, data.uniqueId);
  }
}
