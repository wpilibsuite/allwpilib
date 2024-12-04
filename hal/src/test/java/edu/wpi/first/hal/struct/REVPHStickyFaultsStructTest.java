// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.REVPHStickyFaults;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
public class REVPHStickyFaultsStructTest extends StructTestBase<REVPHStickyFaults> {
  public REVPHStickyFaultsStructTest() {
    super(
        new REVPHStickyFaults(
            0xdead // random
            ),
        REVPHStickyFaults.struct);
  }

  @Override
  public void checkEquals(REVPHStickyFaults testData, REVPHStickyFaults data) {
    assertEquals(testData.m_bitfield, data.m_bitfield);
    assertEquals(testData.Brownout, data.Brownout);
    assertEquals(testData.CanBusOff, data.CanBusOff);
    assertEquals(testData.CompressorOpen, data.CompressorOpen);
    assertEquals(testData.FirmwareFault, data.FirmwareFault);
    assertEquals(testData.HasReset, data.HasReset);
    assertEquals(testData.CanWarning, data.CanWarning);
    assertEquals(testData.HardwareFault, data.HardwareFault);
    assertEquals(testData.SolenoidOverCurrent, data.SolenoidOverCurrent);
    assertEquals(testData.CompressorOverCurrent, data.CompressorOverCurrent);
  }
}
