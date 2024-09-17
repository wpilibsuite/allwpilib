// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.filter.LinearFilter;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

@RunWith(Parameterized.class)
public class MotorEncoderTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(MotorEncoderTest.class.getName());

  private static final double MOTOR_RUNTIME = 0.25;

  // private static final List<MotorEncoderFixture> pairs = new
  // ArrayList<MotorEncoderFixture>();
  private static MotorEncoderFixture<?> me = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Constructs the test.
   *
   * @param mef The fixture under test.
   */
  public MotorEncoderTest(MotorEncoderFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;
  }

  @Parameters(name = "{index}: {0}")
  public static Collection<MotorEncoderFixture<?>[]> generateData() {
    // logger.fine("Loading the MotorList");
    return List.of(
        new MotorEncoderFixture<?>[][] {
          {TestBench.getTalonPair()}, {TestBench.getVictorPair()}, {TestBench.getJaguarPair()}
        });
  }

  @Before
  public void setUp() {
    double initialSpeed = me.getMotor().get();
    assertTrue(
        me.getType() + " Did not start with an initial speed of 0 instead got: " + initialSpeed,
        Math.abs(initialSpeed) < 0.001);
    me.setup();
  }

  @After
  public void tearDown() {
    me.reset();
    encodersResetCheck(me);
  }

  @AfterClass
  public static void tearDownAfterClass() {
    // Clean up the fixture after the test
    me.teardown();
    me = null;
  }

  /**
   * Test to ensure that the isMotorWithinRange method is functioning properly. Really only needs to
   * run on one MotorEncoderFixture to ensure that it is working correctly.
   */
  @Test
  public void testIsMotorWithinRange() {
    double range = 0.01;
    assertTrue(me.getType() + " 1", me.isMotorSpeedWithinRange(0.0, range));
    assertTrue(me.getType() + " 2", me.isMotorSpeedWithinRange(0.0, -range));
    assertFalse(me.getType() + " 3", me.isMotorSpeedWithinRange(1.0, range));
    assertFalse(me.getType() + " 4", me.isMotorSpeedWithinRange(-1.0, range));
  }

  /**
   * This test is designed to see if the values of different motors will increment when spun
   * forward.
   */
  @Test
  public void testIncrement() {
    int startValue = me.getEncoder().get();

    me.getMotor().set(0.2);
    Timer.delay(MOTOR_RUNTIME);
    int currentValue = me.getEncoder().get();
    assertTrue(
        me.getType()
            + " Encoder not incremented: start: "
            + startValue
            + "; current: "
            + currentValue,
        startValue < currentValue);
  }

  /**
   * This test is designed to see if the values of different motors will decrement when spun in
   * reverse.
   */
  @Test
  public void testDecrement() {
    int startValue = me.getEncoder().get();

    me.getMotor().set(-0.2);
    Timer.delay(MOTOR_RUNTIME);
    int currentValue = me.getEncoder().get();
    assertTrue(
        me.getType()
            + " Encoder not decremented: start: "
            + startValue
            + "; current: "
            + currentValue,
        startValue > currentValue);
  }

  /** This method test if the counters count when the motors rotate. */
  @Test
  public void testCounter() {
    final int counter1Start = me.getCounters()[0].get();
    final int counter2Start = me.getCounters()[1].get();

    me.getMotor().set(0.75);
    Timer.delay(MOTOR_RUNTIME);
    int counter1End = me.getCounters()[0].get();
    int counter2End = me.getCounters()[1].get();
    assertTrue(
        me.getType()
            + " Counter not incremented: start: "
            + counter1Start
            + "; current: "
            + counter1End,
        counter1Start < counter1End);
    assertTrue(
        me.getType()
            + " Counter not incremented: start: "
            + counter1Start
            + "; current: "
            + counter2End,
        counter2Start < counter2End);
    me.reset();
    encodersResetCheck(me);
  }

  /**
   * Tests to see if you set the speed to something not {@literal <=} 1.0 if the code appropriately
   * throttles the value.
   */
  @Test
  public void testSetHighForwardSpeed() {
    me.getMotor().set(15);
    assertTrue(
        me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get(),
        me.isMotorSpeedWithinRange(1.0, 0.001));
  }

  /**
   * Tests to see if you set the speed to something not {@literal >=} -1.0 if the code appropriately
   * throttles the value.
   */
  @Test
  public void testSetHighReverseSpeed() {
    me.getMotor().set(-15);
    assertTrue(
        me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get(),
        me.isMotorSpeedWithinRange(-1.0, 0.001));
  }

  @Test
  public void testPositionPIDController() {
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
            "PID loop did not reach reference within 10 seconds. The current error was"
                + pidController.getError(),
            pidController.atSetpoint());
      }
    }
  }

  @Test
  public void testVelocityPIDController() {
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
            "PID loop did not reach reference within 10 seconds. The error was: "
                + pidController.getError(),
            pidController.atSetpoint());
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
        me.getType() + " Encoder value was incorrect after reset.", me.getEncoder().get(), 0);
    assertEquals(
        me.getType() + " Motor value was incorrect after reset.", me.getMotor().get(), 0, 0);
    assertEquals(
        me.getType() + " Counter1 value was incorrect after reset.", me.getCounters()[0].get(), 0);
    assertEquals(
        me.getType() + " Counter2 value was incorrect after reset.", me.getCounters()[1].get(), 0);
    Timer.delay(0.5); // so this doesn't fail with the 0.5 second default
    // timeout on the encoders
    assertTrue(
        me.getType() + " Encoder.getStopped() returned false after the motor was reset.",
        me.getEncoder().getStopped());
  }
}
