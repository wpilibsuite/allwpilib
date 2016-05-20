/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import org.junit.Before;
import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

/**
 * Tests the CAN Speed controllers in quad mode.
 *
 * @author jonathanleitschuh
 */
public class CANSpeedQuadEncoderModeTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANPercentQuadEncoderModeTest.class
      .getName());
  /**
   * The stopped value in rev/min.
   */
  private static final double kStoppedValue = 0;
  /**
   * The running value in rev/min.
   */
  private static final double kRunningValue = 50;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Before
  public void setUp() throws Exception {
    getME().getMotor().setSpeedMode(CANJaguar.kQuadEncoder, 360, 0.1f, 0.003f, 0.01f);
    getME().getMotor().enableControl();
    getME().getMotor().set(0.0f);
    /* The motor might still have momentum from the previous test. */
    Timer.delay(kStartupTime);
  }

  @Test
  public void testDefaultSpeed() {
    assertEquals("CAN Jaguar did not start with an initial speed of zero", 0.0f, getME()
        .getMotor()
        .getSpeed(), 0.3f);
  }


  /**
   * Test if we can drive the motor forward in Speed mode and get a position back.
   */
  @Test
  public void testRotateForwardSpeed() {
    double speed = 50.0f;
    double initialPosition = getME().getMotor().getPosition();
    setCANJaguar(2 * kMotorTime, speed);
    assertEquals("The motor did not reach the required speed in speed mode", speed, getME()
        .getMotor().getSpeed(), kEncoderSpeedTolerance);
    assertThat("The motor did not move forward in speed mode", getME().getMotor().getPosition(),
        is(greaterThan(initialPosition)));
  }

  /**
   * Test if we can drive the motor backwards in Speed mode and get a position back.
   */
  @Test
  public void testRotateReverseSpeed() {
    double speed = -50.0f;
    double initialPosition = getME().getMotor().getPosition();
    setCANJaguar(2 * kMotorTime, speed);
    assertEquals("The motor did not reach the required speed in speed mode", speed, getME()
        .getMotor().getSpeed(), kEncoderSpeedTolerance);
    assertThat("The motor did not move in reverse in speed mode", getME().getMotor()
        .getPosition(),
        is(lessThan(initialPosition)));
  }

}
