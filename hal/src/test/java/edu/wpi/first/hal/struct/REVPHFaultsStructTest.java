// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.REVPHFaults;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
public class REVPHFaultsStructTest extends StructTestBase<REVPHFaults> {
  public REVPHFaultsStructTest() {
    super(
        new REVPHFaults(
            0xdead // random
            ),
        REVPHFaults.struct);
  }

  @Override
  public void checkEquals(REVPHFaults testData, REVPHFaults data) {
    assertEquals(testData.m_bitfield, data.m_bitfield);
    assertEquals(testData.Channel0Fault, data.Channel0Fault);
    assertEquals(testData.Channel1Fault, data.Channel1Fault);
    assertEquals(testData.Channel2Fault, data.Channel2Fault);
    assertEquals(testData.Channel3Fault, data.Channel3Fault);
    assertEquals(testData.Channel4Fault, data.Channel4Fault);
    assertEquals(testData.Channel5Fault, data.Channel5Fault);
    assertEquals(testData.Channel6Fault, data.Channel6Fault);
    assertEquals(testData.Channel7Fault, data.Channel7Fault);
    assertEquals(testData.Channel8Fault, data.Channel8Fault);
    assertEquals(testData.Channel9Fault, data.Channel9Fault);
    assertEquals(testData.Channel10Fault, data.Channel10Fault);
    assertEquals(testData.Channel11Fault, data.Channel11Fault);
    assertEquals(testData.Channel12Fault, data.Channel12Fault);
    assertEquals(testData.Channel13Fault, data.Channel13Fault);
    assertEquals(testData.Channel15Fault, data.Channel15Fault);
    assertEquals(testData.m_bitfield, data.m_bitfield);
    assertEquals(testData.Brownout, data.Brownout);
    assertEquals(testData.CompressorOpen, data.CompressorOpen);
    assertEquals(testData.CanWarning, data.CanWarning);
    assertEquals(testData.HardwareFault, data.HardwareFault);
    assertEquals(testData.SolenoidOverCurrent, data.SolenoidOverCurrent);
    assertEquals(testData.CompressorOverCurrent, data.CompressorOverCurrent);
  }
}
