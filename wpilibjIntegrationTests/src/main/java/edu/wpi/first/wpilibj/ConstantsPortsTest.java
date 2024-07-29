// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.Test;

/** Tests for checking our constant and port values. */
public class ConstantsPortsTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(ConstantsPortsTest.class.getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /** kDigitalChannels. */
  @Test
  public void testDigitalChannels() {
    assertEquals(31, SensorUtil.kDigitalChannels);
  }

  /** kAnalogInputChannels. */
  @Test
  public void testAnalogInputChannels() {
    assertEquals(8, SensorUtil.kAnalogInputChannels);
  }

  /** kAnalogOutputChannels. */
  @Test
  public void testAnalogOutputChannels() {
    assertEquals(2, SensorUtil.kAnalogOutputChannels);
  }

  /** kPwmChannels. */
  @Test
  public void testPwmChannels() {
    assertEquals(20, SensorUtil.kPwmChannels);
  }

  /** kRelayChannels. */
  @Test
  public void testRelayChannels() {
    assertEquals(4, SensorUtil.kRelayChannels);
  }
}
