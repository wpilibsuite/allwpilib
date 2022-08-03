// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

/** Test that covers the {@link Compressor}. */
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

  private static PneumaticsControlModule pcm;

  private static DigitalOutput fakePressureSwitch;
  private static AnalogInput fakeCompressor;

  private static DigitalInput fakeSolenoid1;
  private static DigitalInput fakeSolenoid2;

  @BeforeClass
  public static void setUpBeforeClass() {
    pcm = new PneumaticsControlModule();

    fakePressureSwitch = new DigitalOutput(11);
    fakeCompressor = new AnalogInput(1);

    fakeSolenoid1 = new DigitalInput(12);
    fakeSolenoid2 = new DigitalInput(13);
  }

  @AfterClass
  public static void tearDownAfterClass() {
    fakePressureSwitch.close();
    fakeCompressor.close();

    fakeSolenoid1.close();
    fakeSolenoid2.close();
  }

  @Before
  public void reset() {
    pcm.disableCompressor();
    fakePressureSwitch.set(false);
  }

  /** Test if the compressor turns on and off when the pressure switch is toggled. */
  @Test
  public void testPressureSwitch() {
    final double range = 0.5;
    reset();
    pcm.enableCompressorDigital();

    // Turn on the compressor
    fakePressureSwitch.set(true);
    Timer.delay(kCompressorDelayTime);
    assertEquals(
        "Compressor did not turn on when the pressure switch turned on.",
        kCompressorOnVoltage,
        fakeCompressor.getVoltage(),
        range);

    // Turn off the compressor
    fakePressureSwitch.set(false);
    Timer.delay(kCompressorDelayTime);
    assertEquals(
        "Compressor did not turn off when the pressure switch turned off.",
        kCompressorOffVoltage,
        fakeCompressor.getVoltage(),
        range);
  }

  /** Test if the correct solenoids turn on and off when they should. */
  @Test
  public void testSolenoid() {
    reset();

    Solenoid solenoid1 = new Solenoid(PneumaticsModuleType.CTREPCM, 0);
    Solenoid solenoid2 = new Solenoid(PneumaticsModuleType.CTREPCM, 1);

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

    solenoid1.close();
    solenoid2.close();
  }

  /**
   * Test if the correct solenoids turn on and off when they should when used with the
   * DoubleSolenoid class.
   */
  @Test
  public void doubleSolenoid() {
    DoubleSolenoid solenoid = new DoubleSolenoid(PneumaticsModuleType.CTREPCM, 0, 1);

    solenoid.set(DoubleSolenoid.Value.kOff);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertTrue("DoubleSolenoid did not report off", solenoid.get() == DoubleSolenoid.Value.kOff);

    solenoid.set(DoubleSolenoid.Value.kForward);
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertTrue(
        "DoubleSolenoid did not report Forward", solenoid.get() == DoubleSolenoid.Value.kForward);

    solenoid.set(DoubleSolenoid.Value.kReverse);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertTrue(
        "DoubleSolenoid did not report Reverse", solenoid.get() == DoubleSolenoid.Value.kReverse);

    solenoid.close();
  }

  /** Test if the correct solenoids turn on and off when they should. */
  @Test
  public void testOneShot() {
    reset();

    Solenoid solenoid1 = new Solenoid(PneumaticsModuleType.CTREPCM, 0);
    Solenoid solenoid2 = new Solenoid(PneumaticsModuleType.CTREPCM, 1);

    solenoid1.set(false);
    solenoid2.set(false);
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    // Pulse Solenoid #1 on, and turn Solenoid #2 off
    solenoid1.setPulseDuration(2 * kSolenoidDelayTime);
    solenoid1.startPulse();
    solenoid2.set(false);
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertTrue("Solenoid #1 did not report on", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());
    Timer.delay(2 * kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    // Turn Solenoid #1 off, and pulse Solenoid #2 on
    solenoid1.set(false);
    solenoid2.setPulseDuration(2 * kSolenoidDelayTime);
    solenoid2.startPulse();
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertTrue("Solenoid #2 did not report on", solenoid2.get());
    Timer.delay(2 * kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    // Pulse both Solenoids on
    solenoid1.setPulseDuration(2 * kSolenoidDelayTime);
    solenoid2.setPulseDuration(2 * kSolenoidDelayTime);
    solenoid1.startPulse();
    solenoid2.startPulse();
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertTrue("Solenoid #1 did not report on", solenoid1.get());
    assertTrue("Solenoid #2 did not report on", solenoid2.get());
    Timer.delay(2 * kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    // Pulse both Solenoids on with different durations
    solenoid1.setPulseDuration(1.5 * kSolenoidDelayTime);
    solenoid2.setPulseDuration(2.5 * kSolenoidDelayTime);
    solenoid1.startPulse();
    solenoid2.startPulse();
    Timer.delay(kSolenoidDelayTime);
    assertFalse("Solenoid #1 did not turn on", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertTrue("Solenoid #1 did not report on", solenoid1.get());
    assertTrue("Solenoid #2 did not report on", solenoid2.get());
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertFalse("Solenoid #2 did not turn on", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertTrue("Solenoid #2 did not report on", solenoid2.get());
    Timer.delay(kSolenoidDelayTime);
    assertTrue("Solenoid #1 did not turn off", fakeSolenoid1.get());
    assertTrue("Solenoid #2 did not turn off", fakeSolenoid2.get());
    assertFalse("Solenoid #1 did not report off", solenoid1.get());
    assertFalse("Solenoid #2 did not report off", solenoid2.get());

    solenoid1.close();
    solenoid2.close();
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }
}
