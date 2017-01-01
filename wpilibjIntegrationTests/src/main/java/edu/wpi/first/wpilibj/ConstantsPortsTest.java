/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;

/**
 * Tests for checking our constant and port values.
 */
public class ConstantsPortsTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(ConstantsPortsTest.class.getName());

  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * kDigitalChannels.
   */
  @Test
  public void testDigitalChannels() {
    assertEquals(31, SensorBase.kDigitalChannels);
  }

  /**
   * kAnalogInputChannels.
   */
  @Test
  public void testAnalogInputChannels() {
    assertEquals(8, SensorBase.kAnalogInputChannels);
  }

  /**
   * kAnalogOutputChannels.
   */
  @Test
  public void testAnalogOutputChannels() {
    assertEquals(2, SensorBase.kAnalogOutputChannels);
  }

  /**
   * kSolenoidChannels.
   */
  @Test
  public void testSolenoidChannels() {
    assertEquals(8, SensorBase.kSolenoidChannels);
  }

  /**
   * kPwmChannels.
   */
  @Test
  public void testPwmChannels() {
    assertEquals(20, SensorBase.kPwmChannels);
  }

  /**
   * kRelayChannels.
   */
  @Test
  public void testRelayChannels() {
    assertEquals(4, SensorBase.kRelayChannels);
  }

  /**
   * kPDPChannels.
   */
  @Test
  public void testPDPChannels() {
    assertEquals(16, SensorBase.kPDPChannels);
  }

  /**
   * kPDPModules.
   */
  @Test
  public void testPDPModules() {
    assertEquals(63, SensorBase.kPDPModules);
  }

  /**
   * kPCMModules.
   */
  @Test
  public void testPCMModules() {
    assertEquals(63, SensorBase.kPCMModules);
  }
}
