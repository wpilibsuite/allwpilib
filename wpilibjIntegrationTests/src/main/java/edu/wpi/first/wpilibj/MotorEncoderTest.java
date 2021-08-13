// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.filter.LinearFilter;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

public class MotorEncoderTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(MotorEncoderTest.class.getName());

  private static final double MOTOR_RUNTIME = 0.25;

  private static MotorEncoderFixture<?> me = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  public static Collection<MotorEncoderFixture<?>[]> generateData() {
    // logger.fine("Loading the MotorList");
    return Arrays.asList(
        new MotorEncoderFixture<?>[][] {
          {TestBench.getTalonPair()}, {TestBench.getVictorPair()}, {TestBench.getJaguarPair()}
        });
  }

  @BeforeEach
  public void setUp() {
    double initialSpeed = me.getMotor().get();
    assertTrue(
        Math.abs(initialSpeed) < 0.001,
        me.getType() + " Did not start with an initial speed of 0 instead got: " + initialSpeed);
    me.setup();
  }

  @AfterEach
  public void tearDown() {
    me.reset();
    encodersResetCheck(me);
  }

  @AfterAll
  public static void tearDownAfterAll() {
    // Clean up the fixture after the test
    me.teardown();
    me = null;
  }

  /**
   * Test to ensure that the isMotorWithinRange method is functioning properly. Really only needs to
   * run on one MotorEncoderFixture to ensure that it is working correctly.
   */
  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testIsMotorWithinRange(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    double range = 0.01;
    assertTrue(me.isMotorSpeedWithinRange(0.0, range), me.getType() + " 1");
    assertTrue(me.isMotorSpeedWithinRange(0.0, -range), me.getType() + " 2");
    assertFalse(me.isMotorSpeedWithinRange(1.0, range), me.getType() + " 3");
    assertFalse(me.isMotorSpeedWithinRange(-1.0, range), me.getType() + " 4");
  }

  /**
   * This test is designed to see if the values of different motors will increment when spun
   * forward.
   */
  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testIncrement(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    int startValue = me.getEncoder().get();

    me.getMotor().set(0.2);
    Timer.delay(MOTOR_RUNTIME);
    int currentValue = me.getEncoder().get();
    assertTrue(
        startValue < currentValue,
        me.getType()
            + " Encoder not incremented: start: "
            + startValue
            + "; current: "
            + currentValue);
  }

  /**
   * This test is designed to see if the values of different motors will decrement when spun in
   * reverse.
   */
  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testDecrement(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    int startValue = me.getEncoder().get();

    me.getMotor().set(-0.2);
    Timer.delay(MOTOR_RUNTIME);
    int currentValue = me.getEncoder().get();
    assertTrue(
        startValue > currentValue,
        me.getType()
            + " Encoder not decremented: start: "
            + startValue
            + "; current: "
            + currentValue);
  }

  /** This method test if the counters count when the motors rotate. */
  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testCounter(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    final int counter1Start = me.getCounters()[0].get();
    final int counter2Start = me.getCounters()[1].get();

    me.getMotor().set(0.75);
    Timer.delay(MOTOR_RUNTIME);
    int counter1End = me.getCounters()[0].get();
    int counter2End = me.getCounters()[1].get();
    assertTrue(
        counter1Start < counter1End,
        me.getType()
            + " Counter not incremented: start: "
            + counter1Start
            + "; current: "
            + counter1End);
    assertTrue(
        counter2Start < counter2End,
        me.getType()
            + " Counter not incremented: start: "
            + counter1Start
            + "; current: "
            + counter2End);
    me.reset();
    encodersResetCheck(me);
  }

  /**
   * Tests to see if you set the speed to something not {@literal <=} 1.0 if the code appropriately
   * throttles the value.
   */
  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testSetHighForwardSpeed(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    me.getMotor().set(15);
    assertTrue(
        me.isMotorSpeedWithinRange(1.0, 0.001),
        me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get());
  }

  /**
   * Tests to see if you set the speed to something not {@literal >=} -1.0 if the code appropriately
   * throttles the value.
   */
  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testSetHighReverseSpeed(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    me.getMotor().set(-15);
    assertTrue(
        me.isMotorSpeedWithinRange(-1.0, 0.001),
        me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get());
  }

  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testPositionPIDController(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    try (PIDController pidController = new PIDController(0.001, 0.0005, 0)) {
      pidController.setTolerance(50.0);
      pidController.setIntegratorRange(-0.2, 0.2);
      pidController.setSetpoint(1000);

      try (Notifier pidRunner =
          new Notifier(
              () -> {
                var speed = pidController.calculate(me.getEncoder().getDistance());
                me.getMotor().set(MathUtil.clamp(speed, -0.2, 0.2));
              })) {
        pidRunner.startPeriodic(pidController.getPeriod());
        Timer.delay(10.0);
        pidRunner.stop();

        assertTrue(
            pidController.atSetpoint(),
            "PID loop did not reach reference within 10 seconds. The current error was"
                + pidController.getPositionError());
      }
    }
  }

  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testVelocityPIDController(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;

    LinearFilter filter = LinearFilter.movingAverage(50);
    try (PIDController pidController = new PIDController(1e-5, 0.0, 0.0006)) {
      pidController.setTolerance(200);
      pidController.setSetpoint(600);

      try (Notifier pidRunner =
          new Notifier(
              () -> {
                var speed = filter.calculate(me.getEncoder().getRate());
                me.getMotor().set(MathUtil.clamp(speed, -0.3, 0.3));
              })) {
        pidRunner.startPeriodic(pidController.getPeriod());
        Timer.delay(10.0);
        pidRunner.stop();

        assertTrue(
            pidController.atSetpoint(),
            "PID loop did not reach reference within 10 seconds. The error was: "
                + pidController.getPositionError());
      }
    }
  }

  /**
   * Checks to see if the encoders and counters are appropriately reset to zero when reset.
   *
   * @param me The MotorEncoderFixture under test
   */
  private void encodersResetCheck(MotorEncoderFixture<?> me) {
    assertEquals(
        me.getEncoder().get(), 0, me.getType() + " Encoder value was incorrect after reset.");
    assertEquals(
        me.getMotor().get(), 0, 0, me.getType() + " Motor value was incorrect after reset.");
    assertEquals(
        me.getCounters()[0].get(), 0, me.getType() + " Counter1 value was incorrect after reset.");
    assertEquals(
        me.getCounters()[1].get(), 0, me.getType() + " Counter2 value was incorrect after reset.");
    Timer.delay(0.5); // so this doesn't fail with the 0.5 second default
    // timeout on the encoders
    assertTrue(
        me.getEncoder().getStopped(),
        me.getType() + " Encoder.getStopped() returned false after the motor was reset.");
  }
}
