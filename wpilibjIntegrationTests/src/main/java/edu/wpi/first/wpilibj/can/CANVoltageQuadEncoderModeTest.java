/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import com.googlecode.junittoolbox.PollingWait;
import com.googlecode.junittoolbox.RunnableAssert;

import org.junit.Before;
import org.junit.Test;

import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

/**
 * Tests the CAN motor controllers in voltage mode work correctly.
 *
 * @author Jonathan Leitschuh
 */
public class CANVoltageQuadEncoderModeTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANVoltageQuadEncoderModeTest.class
      .getName());
  /**
   * The stopped value in volts.
   */
  private static final double kStoppedValue = 0;
  /**
   * The running value in volts.
   */
  private static final double kRunningValue = 4;

  private static final double kVoltageTolerance = .25;

  private static final PollingWait kWait = new PollingWait().timeoutAfter(
      (long) kMotorTimeSettling, TimeUnit.SECONDS).pollEvery(1, TimeUnit.MILLISECONDS);

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#stopMotor()
   */
  protected void stopMotor() {
    getME().getMotor().set(kStoppedValue);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorForward()
   */
  protected void runMotorForward() {
    getME().getMotor().set(kRunningValue);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorReverse()
   */
  protected void runMotorReverse() {
    getME().getMotor().set(-kRunningValue);
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Sets up the test.
   */
  @Before
  public void setUp() {
    getME().getMotor().setVoltageMode(CANJaguar.kQuadEncoder, 360);
    getME().getMotor().set(kStoppedValue);
    getME().getMotor().enableControl();

    /* The motor might still have momentum from the previous test. */
    Timer.delay(kStartupTime);
  }

  @Test
  public void testRotateForwardToVoltage() {
    setCANJaguar(kMotorTime, Math.PI);
    assertEquals("The output voltage did not match the desired voltage set-point", Math.PI, getME()
        .getMotor().getOutputVoltage(), kVoltageTolerance);
  }

  @Test
  public void testRotateReverseToVoltage() {
    setCANJaguar(kMotorTime, -Math.PI);
    assertEquals("The output voltage did not match the desired voltage set-point", -Math.PI,
        getME().getMotor().getOutputVoltage(), kVoltageTolerance);
  }


  /**
   * Sets up the test to have the CANJaguar running at the target voltage.
   *
   * @param targetValue the target voltage
   * @param wait        the PollingWait to to use to wait for the setup to complete with
   */
  private void setupMotorVoltageForTest(final double targetValue, PollingWait wait) {
    getME().getMotor().enableControl();
    setCANJaguar(1, targetValue);
    wait.until(new RunnableAssert(
        "[SETUP] Waiting for the output voltage to match the set output value") {
      @Override
      public void run() throws Exception {
        getME().getMotor().set(targetValue);
        assertEquals("[TEST SETUP] The output voltage should have matched the set value",
            targetValue, getME().getMotor().getOutputVoltage(), 0.5);
        assertEquals("[TEST SETUP] The set value did not match the get value", targetValue, getME()
            .getMotor().get(), 0.5);
      }
    });
  }


  @Test
  public void testMaxOutputVoltagePositive() {
    // given
    double maxVoltage = 3;

    setupMotorVoltageForTest(kRunningValue, kWait);

    final double fastSpeed = getME().getMotor().getSpeed();

    // when
    getME().getMotor().configMaxOutputVoltage(maxVoltage);

    setCANJaguar(1, kRunningValue);
    // Then
    kWait.until(new RunnableAssert("Waiting for the speed to reduce using max output voltage") {
      @Override
      public void run() throws Exception {
        runMotorForward();
        assertThat("Speed did not reduce when the max output voltage was set", fastSpeed,
            is(greaterThan(getME().getMotor().getSpeed())));
      }
    });

  }

  @Test
  public void testMaxOutputVoltagePositiveSetToZeroStopsMotor() {
    // given
    final double maxVoltage = 0;

    setupMotorVoltageForTest(kRunningValue, kWait);
    // when
    getME().getMotor().configMaxOutputVoltage(maxVoltage);

    setCANJaguar(1, kRunningValue);
    // then
    kWait.until(new RunnableAssert(
        "Waiting for the speed to reduce to zero using max output voltage") {
      @Override
      public void run() throws Exception {
        runMotorForward();
        assertEquals("Speed did not go to zero when the max output voltage was set to "
            + maxVoltage, 0, getME().getMotor().getSpeed(), kEncoderSpeedTolerance);
      }
    });
  }


  @Test
  public void testMaxOutputVoltageNegative() {
    // given
    double maxVoltage = 3;

    setupMotorVoltageForTest(-kRunningValue, kWait);
    final double fastSpeed = getME().getMotor().getSpeed();

    // when
    getME().getMotor().configMaxOutputVoltage(maxVoltage);
    setCANJaguar(1, -kRunningValue);

    // then
    kWait.until(new RunnableAssert("Waiting for the speed to reduce using max output voltage") {

      @Override
      public void run() throws Exception {
        runMotorReverse();
        assertThat("Speed did not reduce when the max output voltage was set", fastSpeed,
            is(lessThan(getME().getMotor().getSpeed())));
      }
    });
  }

  @Test
  public void testMaxOutputVoltageNegativeSetToZeroStopsMotor() {
    // given
    final double maxVoltage = 0;
    setupMotorVoltageForTest(-kRunningValue, kWait);

    // when
    getME().getMotor().configMaxOutputVoltage(maxVoltage);
    setCANJaguar(1, -kRunningValue);

    // Then
    kWait.until(new RunnableAssert(
        "Waiting for the speed to reduce to zero using max output voltage") {
      @Override
      public void run() throws Exception {
        runMotorReverse();
        assertEquals("Speed did not go to zero when the max output voltage was set to "
            + maxVoltage, 0, getME().getMotor().getSpeed(), kEncoderSpeedTolerance);
      }
    });
  }

}
