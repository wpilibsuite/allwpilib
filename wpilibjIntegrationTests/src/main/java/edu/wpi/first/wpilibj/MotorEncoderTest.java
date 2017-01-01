/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;


@RunWith(Parameterized.class)
public class MotorEncoderTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(MotorEncoderTest.class.getName());

  private static final double MOTOR_RUNTIME = .25;

  // private static final List<MotorEncoderFixture> pairs = new
  // ArrayList<MotorEncoderFixture>();
  private static MotorEncoderFixture<?> me = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Constructs the test
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
    return Arrays.asList(new MotorEncoderFixture<?>[][]{{TestBench.getInstance().getTalonPair()},
        {TestBench.getInstance().getVictorPair()}, {TestBench.getInstance().getJaguarPair()}});
  }

  @Before
  public void setUp() {
    double initialSpeed = me.getMotor().get();
    assertTrue(me.getType() + " Did not start with an initial speed of 0 instead got: "
        + initialSpeed, Math.abs(initialSpeed) < 0.001);
    me.setup();

  }

  @After
  public void tearDown() throws Exception {
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

    me.getMotor().set(.2);
    Timer.delay(MOTOR_RUNTIME);
    int currentValue = me.getEncoder().get();
    assertTrue(me.getType() + " Encoder not incremented: start: " + startValue + "; current: "
        + currentValue, startValue < currentValue);

  }

  /**
   * This test is designed to see if the values of different motors will decrement when spun in
   * reverse.
   */
  @Test
  public void testDecrement() {
    int startValue = me.getEncoder().get();

    me.getMotor().set(-.2);
    Timer.delay(MOTOR_RUNTIME);
    int currentValue = me.getEncoder().get();
    assertTrue(me.getType() + " Encoder not decremented: start: " + startValue + "; current: "
        + currentValue, startValue > currentValue);
  }

  /**
   * This method test if the counters count when the motors rotate.
   */
  @Test
  public void testCounter() {
    final int counter1Start = me.getCounters()[0].get();
    final int counter2Start = me.getCounters()[1].get();

    me.getMotor().set(.75);
    Timer.delay(MOTOR_RUNTIME);
    int counter1End = me.getCounters()[0].get();
    int counter2End = me.getCounters()[1].get();
    assertTrue(me.getType() + " Counter not incremented: start: " + counter1Start + "; current: "
        + counter1End, counter1Start < counter1End);
    assertTrue(me.getType() + " Counter not incremented: start: " + counter1Start + "; current: "
        + counter2End, counter2Start < counter2End);
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
    assertTrue(me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get(),
        me.isMotorSpeedWithinRange(1.0, 0.001));
  }

  /**
   * Tests to see if you set the speed to something not {@literal >=} -1.0 if the code appropriately
   * throttles the value.
   */
  @Test
  public void testSetHighReverseSpeed() {
    me.getMotor().set(-15);
    assertTrue(me.getType() + " Motor speed was not close to 1.0, was: " + me.getMotor().get(),
        me.isMotorSpeedWithinRange(-1.0, 0.001));
  }


  @Test
  public void testPositionPIDController() {
    me.getEncoder().setPIDSourceType(PIDSourceType.kDisplacement);
    PIDController pid = new PIDController(0.001, 0.0005, 0, me.getEncoder(), me.getMotor());
    pid.setAbsoluteTolerance(50.0);
    pid.setOutputRange(-0.2, 0.2);
    pid.setSetpoint(1000);

    pid.enable();
    Timer.delay(10.0);
    pid.disable();

    assertTrue(
        "PID loop did not reach setpoint within 10 seconds. The average error was: " + pid
            .getAvgError() + "The current error was" + pid.getError(), pid.onTarget());

    pid.free();
  }

  @Test
  public void testVelocityPIDController() {
    me.getEncoder().setPIDSourceType(PIDSourceType.kRate);
    PIDController pid =
        new PIDController(1e-5, 0.0, 3e-5, 8e-5, me.getEncoder(), me.getMotor());
    pid.setAbsoluteTolerance(200);
    pid.setToleranceBuffer(50);
    pid.setOutputRange(-0.3, 0.3);
    pid.setSetpoint(600);

    pid.enable();
    Timer.delay(10.0);
    pid.disable();

    assertTrue(
        "PID loop did not reach setpoint within 10 seconds. The error was: " + pid.getAvgError(),
        pid.onTarget());

    pid.free();
  }

  /**
   * Checks to see if the encoders and counters are appropriately reset to zero when reset.
   *
   * @param me The MotorEncoderFixture under test
   */
  private void encodersResetCheck(MotorEncoderFixture<?> me) {
    assertEquals(me.getType() + " Encoder value was incorrect after reset.", me.getEncoder().get(),
        0);
    assertEquals(me.getType() + " Motor value was incorrect after reset.", me.getMotor().get(), 0,
        0);
    assertEquals(me.getType() + " Counter1 value was incorrect after reset.",
        me.getCounters()[0].get(), 0);
    assertEquals(me.getType() + " Counter2 value was incorrect after reset.",
        me.getCounters()[1].get(), 0);
    Timer.delay(0.5); // so this doesn't fail with the 0.5 second default
    // timeout on the encoders
    assertTrue(me.getType() + " Encoder.getStopped() returned false after the motor was reset.", me
        .getEncoder().getStopped());
  }

}
