/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class GyroTest extends AbstractComsSetup {

  private static final Logger logger = Logger.getLogger(GyroTest.class.getName());

  public static final double TEST_ANGLE = 90.0;

  private TiltPanCameraFixture tpcam;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Before
  public void setUp() throws Exception {
    logger.fine("Setup: TiltPan camera");
    tpcam = TestBench.getInstance().getTiltPanCam();
    tpcam.setup();
  }

  @After
  public void tearDown() throws Exception {
    tpcam.teardown();
  }

  @Test
  public void testAllGyroTests() {
    testInitial();
    testGyroAngle();
    testDeviationOverTime();
    testGyroAngleCalibratedParameters();
  }

  public void testInitial() {
    double angle = tpcam.getGyro().getAngle();
    assertEquals(errorMessage(angle, 0), 0, angle, .5);
  }

  /**
   * Test to see if the Servo and the gyroscope is turning 90 degrees Note servo
   * on TestBench is not the same type of servo that servo class was designed
   * for so setAngle is significantly off. This has been calibrated for the
   * servo on the rig.
   */
  public void testGyroAngle() {
    // Set angle
    for (int i = 0; i < 5; i++) {
      tpcam.getPan().set(0);
      Timer.delay(.1);
    }

    Timer.delay(0.5);
    // Reset for setup
    tpcam.getGyro().reset();
    Timer.delay(0.5);

    // Perform test
    for (int i = 0; i < 53; i++) {
      tpcam.getPan().set(i / 100.0);
      Timer.delay(0.05);
    }
    Timer.delay(1.2);

    double angle = tpcam.getGyro().getAngle();

    double difference = TEST_ANGLE - angle;

    double diff = Math.abs(difference);

    assertEquals(errorMessage(diff, TEST_ANGLE), TEST_ANGLE, angle, 10);
  }

  public void testDeviationOverTime() {
    // Make sure that the test isn't influenced by any previous motions.
    Timer.delay(0.5);
    tpcam.getGyro().reset();
    Timer.delay(0.25);
    double angle = tpcam.getGyro().getAngle();
    assertEquals(errorMessage(angle, 0), 0, angle, .5);
    Timer.delay(5);
    angle = tpcam.getGyro().getAngle();
    assertEquals("After 5 seconds " + errorMessage(angle, 0), 0, angle, 1);
  }

  /**
   *  Gets calibrated parameters from already calibrated gyro, allocates a
   *  new gyro with the center and offset parameters, and re-runs the test.
   */
  public void testGyroAngleCalibratedParameters() {
    // Get calibrated parameters to make new Gyro with parameters
    double cOffset = tpcam.getGyro().getOffset();
    int cCenter = tpcam.getGyro().getCenter();
    tpcam.freeGyro();
    tpcam.setupGyroParam(cCenter, cOffset);

    // Repeat tests
    // Set angle
    for (int i = 0; i < 5; i++) {
      tpcam.getPan().set(0);
      Timer.delay(.1);
    }

    Timer.delay(0.5);

    // Reset for setup
    tpcam.getGyroParam().reset();
    Timer.delay(0.5);

    // Perform test
    for (int i = 0; i < 53; i++) {
      tpcam.getPan().set(i / 100.0);
      Timer.delay(0.05);
    }
    Timer.delay(1.2);

    double angle = tpcam.getGyroParam().getAngle();

    double difference = TEST_ANGLE - angle;

    double diff = Math.abs(difference);

    assertEquals(errorMessage(diff, TEST_ANGLE), TEST_ANGLE, angle, 10);
  }

  private String errorMessage(double difference, double target) {
    return "Gyro angle skewed " + difference + " deg away from target " + target;
  }

}
