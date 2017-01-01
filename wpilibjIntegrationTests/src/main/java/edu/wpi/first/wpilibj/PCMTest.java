/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * Test that covers the {@link Compressor}.
 */

public class PCMTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PCMTest.class.getName());
  /*
   * The PCM switches the compressor up to 2 seconds after the pressure switch
   * changes.
   */
  protected static final double kCompressorDelayTime = 2.0;

  /* Solenoids should change much more quickly */
  protected static final double kSolenoidDelayTime = 1.0;

  /*
   * The voltage divider on the test bench should bring the compressor output to
   * around these values.
   */
  protected static final double kCompressorOnVoltage = 5.00;
  protected static final double kCompressorOffVoltage = 1.68;

  private static Compressor compressor;

  private static DigitalOutput fakePressureSwitch;
  private static AnalogInput fakeCompressor;

  private static DigitalInput fakeSolenoid1;
  private static DigitalInput fakeSolenoid2;

  @BeforeClass
  public static void setUpBeforeClass() throws Exception {
    compressor = new Compressor();

    fakePressureSwitch = new DigitalOutput(11);
    fakeCompressor = new AnalogInput(1);

    fakeSolenoid1 = new DigitalInput(12);
    fakeSolenoid2 = new DigitalInput(13);
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
    compressor.free();

    fakePressureSwitch.free();
    fakeCompressor.free();

    fakeSolenoid1.free();
    fakeSolenoid2.free();
  }

  @Before
  public void reset() throws Exception {
    compressor.stop();
    fakePressureSwitch.set(false);
  }

  /**
   * Test if the compressor turns on and off when the pressure switch is toggled.
   */
  @Test
  public void testPressureSwitch() throws Exception {
    final double range = 0.1;
    reset();
    compressor.setClosedLoopControl(true);

    // Turn on the compressor
    fakePressureSwitch.set(true);
    Timer.delay(kCompressorDelayTime);
    assertEquals("Compressor did not turn on when the pressure switch turned on.",
        kCompressorOnVoltage, fakeCompressor.getVoltage(), range);

    // Turn off the compressor
    fakePressureSwitch.set(false);
    Timer.delay(kCompressorDelayTime);
    assertEquals("Compressor did not turn off when the pressure switch turned off.",
        kCompressorOffVoltage, fakeCompressor.getVoltage(), range);
  }

  /**
   * Test if the correct solenoids turn on and off when they should.
   */
  @Test
  public void testSolenoid() throws Exception {
    reset();

    Solenoid solenoid1 = new Solenoid(0);
    Solenoid solenoid2 = new Solenoid(1);

    solenoid1.set(false);
    solenoid2.set(false);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    // Turn Solenoid #1 on, and turn Solenoid #2 off
    solenoid1.set(true);
    solenoid2.set(false);
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertTrue("Solenoid #1 did not report on", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    // Turn Solenoid #1 off, and turn Solenoid #2 on
    solenoid1.set(false);
    solenoid2.set(true);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertTrue("Solenoid #2 did not report on", solenoid2.get());

    // Turn both Solenoids on
    solenoid1.set(true);
    solenoid2.set(true);
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertTrue("Solenoid #1 did not report on", solenoid1.get());
    assertTrue("Solenoid #2 did not report on", solenoid2.get());

    solenoid1.free();
    solenoid2.free();
  }

  /**
   * Test if the correct solenoids turn on and off when they should when used with the
   * DoubleSolenoid class.
   */
  @Test
  public void doubleSolenoid() {
    DoubleSolenoid solenoid = new DoubleSolenoid(0, 1);

    solenoid.set(DoubleSolenoid.Value.kOff);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertTrue("DoubleSolenoid did not report off", (solenoid.get() == DoubleSolenoid.Value.kOff));

    solenoid.set(DoubleSolenoid.Value.kForward);
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertTrue("DoubleSolenoid did not report Forward", (solenoid.get() == DoubleSolenoid.Value
        .kForward));

    solenoid.set(DoubleSolenoid.Value.kReverse);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertTrue("DoubleSolenoid did not report Reverse", (solenoid.get() == DoubleSolenoid.Value
        .kReverse));

    solenoid.free();
  }

  protected Logger getClassLogger() {
    return logger;
  }
}
