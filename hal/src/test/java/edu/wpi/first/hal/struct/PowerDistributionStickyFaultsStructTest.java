// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.PowerDistributionStickyFaults;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
public class PowerDistributionStickyFaultsStructTest
    extends StructTestBase<PowerDistributionStickyFaults> {
  public PowerDistributionStickyFaultsStructTest() {
    super(
        new PowerDistributionStickyFaults(
            0xdead // random
            ),
        PowerDistributionStickyFaults.struct);
  }

  @Override
  public void checkEquals(
      PowerDistributionStickyFaults testData, PowerDistributionStickyFaults data) {
    assertEquals(testData.m_bitfield, data.m_bitfield);
    assertEquals(testData.Channel0BreakerFault, data.Channel0BreakerFault);
    assertEquals(testData.Channel1BreakerFault, data.Channel1BreakerFault);
    assertEquals(testData.Channel2BreakerFault, data.Channel2BreakerFault);
    assertEquals(testData.Channel3BreakerFault, data.Channel3BreakerFault);
    assertEquals(testData.Channel4BreakerFault, data.Channel4BreakerFault);
    assertEquals(testData.Channel5BreakerFault, data.Channel5BreakerFault);
    assertEquals(testData.Channel6BreakerFault, data.Channel6BreakerFault);
    assertEquals(testData.Channel7BreakerFault, data.Channel7BreakerFault);
    assertEquals(testData.Channel8BreakerFault, data.Channel8BreakerFault);
    assertEquals(testData.Channel9BreakerFault, data.Channel9BreakerFault);
    assertEquals(testData.Channel10BreakerFault, data.Channel10BreakerFault);
    assertEquals(testData.Channel11BreakerFault, data.Channel11BreakerFault);
    assertEquals(testData.Channel12BreakerFault, data.Channel12BreakerFault);
    assertEquals(testData.Channel13BreakerFault, data.Channel13BreakerFault);
    assertEquals(testData.Channel15BreakerFault, data.Channel15BreakerFault);
    assertEquals(testData.Channel16BreakerFault, data.Channel16BreakerFault);
    assertEquals(testData.Channel17BreakerFault, data.Channel17BreakerFault);
    assertEquals(testData.Channel18BreakerFault, data.Channel18BreakerFault);
    assertEquals(testData.Channel19BreakerFault, data.Channel19BreakerFault);
    assertEquals(testData.Channel20BreakerFault, data.Channel20BreakerFault);
    assertEquals(testData.Channel21BreakerFault, data.Channel21BreakerFault);
    assertEquals(testData.Channel22BreakerFault, data.Channel22BreakerFault);
    assertEquals(testData.Channel23BreakerFault, data.Channel23BreakerFault);
    assertEquals(testData.Brownout, data.Brownout);
    assertEquals(testData.CanWarning, data.CanWarning);
    assertEquals(testData.HardwareFault, data.HardwareFault);
  }
}
