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
import org.junit.Ignore;
import org.junit.Test;

import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;

/**
 * Tests the CAN motor in PercentQuadEncoderMode.
 *
 * @author jonathanleitschuh
 */
@SuppressWarnings("AbbreviationAsWordInName")
public class CANPercentQuadEncoderModeTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANPercentQuadEncoderModeTest.class
      .getName());
  private static final double kStoppedValue = 0;
  private static final double kRunningValue = 0.3;

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

  @Before
  public void setUp() {
    getME().getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
    getME().getMotor().enableControl();
    getME().getMotor().set(0.0f);
    /* The motor might still have momentum from the previous test. */
    Timer.delay(kStartupTime);
  }

  @Test
  public void testDisableStopsTheMotor() {
    // given
    getME().getMotor().enableControl();
    setCANJaguar(kMotorTime / 2, 1);
    getME().getMotor().disableControl();
    // when
    simpleLog(Level.FINER, "The motor should stop running now");
    setCANJaguar(kMotorTime / 2, 1);
    final double initialPosition = getME().getMotor().getPosition();
    setCANJaguar(kMotorTime / 2, 1);

    // then
    assertEquals("Speed did not go to zero when disabled in percent mode", 0, getME().getMotor()
        .getSpeed(), kEncoderSpeedTolerance);
    assertEquals(initialPosition, getME().getMotor().getPosition(), 10);
  }

  @Test
  public void testRotateForward() {
    // Given
    getME().getMotor().enableControl();
    final double initialPosition = getME().getMotor().getPosition();
    // When
    /* Drive the speed controller briefly to move the encoder */
    runMotorForward();

    // Then
    PollingWait wait =
        new PollingWait().timeoutAfter((long) kMotorTimeSettling, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    wait.until(new RunnableAssert("CANJaguar position incrementing") {
      @Override
      public void run() throws Exception {
        runMotorForward();
        assertThat("CANJaguar position should have increased after the motor moved", getME()
            .getMotor().getPosition(), is(greaterThan(initialPosition)));
      }
    });

    stopMotor();
  }

  @Test
  public void testRotateReverse() {
    // Given
    getME().getMotor().enableControl();
    final double initialPosition = getME().getMotor().getPosition();
    // When
    /* Drive the speed controller briefly to move the encoder */
    runMotorReverse();

    // Then
    PollingWait wait =
        new PollingWait().timeoutAfter((long) kMotorTimeSettling, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    wait.until(new RunnableAssert("CANJaguar position decrementing") {
      @Override
      public void run() throws Exception {
        runMotorReverse();
        assertThat("CANJaguar position should have decreased after the motor moved", getME()
            .getMotor().getPosition(), is(lessThan(initialPosition)));
      }
    });
    stopMotor();
  }

  /**
   * Test if we can limit the Jaguar to not rotate forwards when the fake limit switch is tripped.
   */
  @Test
  public void shouldNotRotateForwards_WhenFakeLimitSwitchForwardsIsTripped() {
    // Given
    getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
    getME().getForwardLimit().set(true);
    getME().getReverseLimit().set(false);
    getME().getMotor().enableControl();

    stopMotor();
    Timer.delay(kEncoderSettlingTime);

    PollingWait wait =
        new PollingWait().timeoutAfter((long) kLimitSettlingTime, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    /* Wait until the limits are recognized by the CANJaguar. */
    wait.until(new RunnableAssert(
        "Waiting for the forward and reverse limit switches to be in the correct state") {
      @Override
      public void run() throws Exception {
        stopMotor();
        assertFalse("[TEST SETUP] The forward limit switch is not in the correct state", getME()
            .getMotor().getForwardLimitOK());
        assertTrue("[TEST SETUP]The reverse limit switch is not in the correct state", getME()
            .getMotor().getReverseLimitOK());
      }
    });

    final double initialPosition = getME().getMotor().getPosition();

    // When
    /*
     * Drive the speed controller briefly to move the encoder. If the limit
     * switch is recognized, it shouldn't actually move.
     */
    setCANJaguar(kMotorTime, 1);
    stopMotor();

    // Then
    /* The position should be the same, since the limit switch was on. */
    assertEquals("CAN Jaguar should not have moved with the forward limit switch pressed",
        initialPosition, getME().getMotor().getPosition(), kEncoderPositionTolerance);
  }


  /**
   * Test if we can rotate in reverse when the limit switch.
   */
  @Test
  public void shouldRotateReverse_WhenFakeLimitSwitchForwardsIsTripped() {
    // Given
    getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
    getME().getForwardLimit().set(true);
    getME().getReverseLimit().set(false);
    getME().getMotor().enableControl();

    stopMotor();
    Timer.delay(kEncoderSettlingTime);

    PollingWait limitWait =
        new PollingWait().timeoutAfter((long) kLimitSettlingTime, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    /* Wait until the limits are recognized by the CANJaguar. */
    limitWait.until(new RunnableAssert(
        "Waiting for the forward and reverse limit switches to be in the correct state") {
      @Override
      public void run() throws Exception {
        stopMotor();
        assertFalse("[TEST SETUP] The forward limit switch is not in the correct state", getME()
            .getMotor().getForwardLimitOK());
        assertTrue("[TEST SETUP] The reverse limit switch is not in the correct state", getME()
            .getMotor().getReverseLimitOK());
      }
    });

    final double initialPosition = getME().getMotor().getPosition();

    // When
    /*
     * Drive the speed controller in the other direction. It should actually
     * move, since only the forward switch is activated.
     */
    setCANJaguar(kMotorTime, -1);
    // Then
    PollingWait wait =
        new PollingWait().timeoutAfter((long) kMotorTimeSettling, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    wait.until(new RunnableAssert("Waiting for the encoder to update") {
      @Override
      public void run() throws Exception {
        runMotorReverse();
        assertThat("CAN Jaguar should have moved in reverse while the forward limit was on",
            getME().getMotor().getPosition(), is(lessThan(initialPosition)));
      }

    });
    stopMotor();

  }

  /**
   * Test if we can limit the Jaguar to only moving forwards with a fake limit switch.
   */
  @Test
  public void shouldNotRotateReverse_WhenFakeLimitSwitchReversesIsTripped() {
    // Given
    getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
    getME().getForwardLimit().set(false);
    getME().getReverseLimit().set(true);
    getME().getMotor().enableControl();

    stopMotor();
    Timer.delay(kEncoderSettlingTime);

    PollingWait wait =
        new PollingWait().timeoutAfter((long) kLimitSettlingTime, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    /* Wait until the limits are recognized by the CANJaguar. */
    wait.until(new RunnableAssert(
        "Waiting for the forward and reverse limit switches to be in the correct state") {
      @Override
      public void run() throws Exception {
        stopMotor();
        assertTrue("[TEST SETUP] The forward limit switch is not in the correct state", getME()
            .getMotor().getForwardLimitOK());
        assertFalse("[TEST SETUP] The reverse limit switch is not in the correct state", getME()
            .getMotor().getReverseLimitOK());
      }
    });

    final double initialPosition = getME().getMotor().getPosition();

    // When
    /*
     * Drive the speed controller backwards briefly to move the encoder. If the
     * limit switch is recognized, it shouldn't actually move.
     */
    setCANJaguar(kMotorTime, -1);
    stopMotor();

    // Then
    /* The position should be the same, since the limit switch was on. */
    assertEquals("CAN Jaguar should not have moved with the limit switch pressed", initialPosition,
        getME().getMotor().getPosition(), kEncoderPositionTolerance);
  }

  /**
   * Test if we can limit the Jaguar to only moving forwards with a fake limit
   * switch.
   */
  @Test
  public void shouldRotateForward_WhenFakeLimitSwitchReversesIsTripped() {
    // Given
    getME().getMotor().configLimitMode(CANJaguar.LimitMode.SwitchInputsOnly);
    getME().getForwardLimit().set(false);
    getME().getReverseLimit().set(true);
    getME().getMotor().enableControl();

    PollingWait limitWait =
        new PollingWait().timeoutAfter((long) kLimitSettlingTime, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    /* Wait until the limits are recognized by the CANJaguar. */
    limitWait.until(new RunnableAssert(
        "Waiting for the forward and reverse limit switches to be in the correct state") {
      @Override
      public void run() throws Exception {
        stopMotor();
        assertTrue("[TEST SETUP] The forward limit switch is not in the correct state", getME()
            .getMotor().getForwardLimitOK());
        assertFalse("[TEST SETUP] The reverse limit switch is not in the correct state", getME()
            .getMotor().getReverseLimitOK());
      }
    });

    final double initialPosition = getME().getMotor().getPosition();

    // When
    /*
     * Drive the speed controller in the other direction. It should actually
     * move, since only the reverse switch is activated.
     */
    setCANJaguar(kMotorTime, 1);
    // Then
    /* The position should have increased */
    PollingWait wait =
        new PollingWait().timeoutAfter((long) kMotorTimeSettling, TimeUnit.SECONDS).pollEvery(1,
            TimeUnit.MILLISECONDS);
    wait.until(new RunnableAssert("Waiting for the encoder to update") {
      @Override
      public void run() throws Exception {
        runMotorForward();
        assertThat("CAN Jaguar should have moved forwards while the reverse limit was on", getME()
            .getMotor().getPosition(), is(greaterThan(initialPosition)));
      }

    });
    stopMotor();
  }

  @Ignore("Encoder is not yet wired to the FPGA")
  @Test
  public void testRotateForwardEncoderToFPGA() {
    getME().getMotor().enableControl();
    final double jagInitialPosition = getME().getMotor().getPosition();
    final double encoderInitialPosition = getME().getEncoder().get();
    getME().getMotor().set(1);
    Timer.delay(kMotorStopTime);
    getME().getMotor().set(0);

    delayTillInCorrectStateWithMessage(Level.FINE, kEncoderSettlingTime,
        "Forward Encodeder settling", new BooleanCheck() {
          @Override
          public boolean getAsBoolean() {
            return Math.abs((getME().getMotor().getPosition() - jagInitialPosition)
                - (getME().getEncoder().get() - encoderInitialPosition))
                < kEncoderPositionTolerance;
          }
        });

    assertEquals(getME().getMotor().getPosition() - jagInitialPosition, getME().getEncoder().get()
        - encoderInitialPosition, kEncoderPositionTolerance);
  }

  @Ignore("Encoder is not yet wired to the FPGA")
  @Test
  public void testRotateReverseEncoderToFPGA() {
    getME().getMotor().enableControl();
    final double jagInitialPosition = getME().getMotor().getPosition();
    final double encoderInitialPosition = getME().getEncoder().get();
    getME().getMotor().set(-1);
    Timer.delay(kMotorStopTime);
    getME().getMotor().set(0);

    delayTillInCorrectStateWithMessage(Level.FINE, kEncoderSettlingTime,
        "Forward Encodeder settling", new BooleanCheck() {
          @Override
          public boolean getAsBoolean() {
            return Math.abs((getME().getMotor().getPosition() - jagInitialPosition)
                - (getME().getEncoder().get() - encoderInitialPosition))
                < kEncoderPositionTolerance;
          }
        });
    assertEquals(getME().getMotor().getPosition() - jagInitialPosition, getME().getEncoder().get()
        - encoderInitialPosition, kEncoderPositionTolerance);
  }
}
