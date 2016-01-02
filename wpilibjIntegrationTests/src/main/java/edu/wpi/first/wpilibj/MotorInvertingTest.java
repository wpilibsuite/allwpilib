/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import edu.wpi.first.wpilibj.test.TestBench;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

/**
 * Tests Inversion of motors using the SpeedController setInverted
 */
@RunWith(Parameterized.class)
public class MotorInvertingTest extends AbstractComsSetup {
  static MotorEncoderFixture<?> fixture = null;
  private static final double motorspeed = 0.35;
  private static final double delaytime = 0.3;


  public MotorInvertingTest(MotorEncoderFixture<?> afixture) {
    logger.fine("Constructor with: " + afixture.getType());
    if (fixture != null && !fixture.equals(afixture))
      fixture.teardown();
    fixture = afixture;
    fixture.setup();
  }

  @Parameterized.Parameters(name = "{index}: {0}")
  public static Collection<MotorEncoderFixture<?>[]> generateData() {
    // logger.fine("Loading the MotorList");
    return Arrays.asList(new MotorEncoderFixture<?>[][] { {TestBench.getInstance().getTalonPair()},
        {TestBench.getInstance().getVictorPair()}, {TestBench.getInstance().getJaguarPair()}});
  }

  private static final Logger logger = Logger.getLogger(MotorInvertingTest.class.getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Before
  public void setUp() {
    // Reset the fixture elements before every test
    fixture.reset();
  }

  @AfterClass
  public static void tearDown() {
    fixture.getMotor().setInverted(false);
    // Clean up the fixture after the test
    fixture.teardown();
  }

  @Test
  public void testInvertingPositive() {
    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    assertFalse("Inverting with Positive value does not change direction", fixture.getEncoder()
        .getDirection() == initDirection);
    fixture.getMotor().set(0);
  }

  @Test
  public void testInvertingNegative() {
    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    assertFalse("Inverting with Negative value does not change direction", fixture.getEncoder()
        .getDirection() == initDirection);
    fixture.getMotor().set(0);
  }

  @Test
  public void testInvertingSwitchingPosToNeg() {
    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    assertTrue("Inverting with Switching value does change direction", fixture.getEncoder()
        .getDirection() == initDirection);
    fixture.getMotor().set(0);
  }

  @Test
  public void testInvertingSwitchingNegToPos() {
    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    assertTrue("Inverting with Switching value does change direction", fixture.getEncoder()
        .getDirection() == initDirection);
    fixture.getMotor().set(0);
  }
}
