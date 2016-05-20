/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

import static org.junit.Assert.assertEquals;

/**
 * Tests the CAN Motor Encoders in QuadEncoder mode.
 *
 * @author jonathanleitschuh
 */
public class CANPositionQuadEncoderModeTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANPositionQuadEncoderModeTest.class
      .getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }


  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorForward()
   */
  protected void runMotorForward() {
    double postion = getME().getMotor().getPosition();
    getME().getMotor().set(postion + 100);
  }


  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorReverse()
   */
  protected void runMotorReverse() {
    double postion = getME().getMotor().getPosition();
    getME().getMotor().set(postion - 100);
  }


  @Before
  public void setUp() throws Exception {
    getME().getMotor().setPositionMode(CANJaguar.kQuadEncoder, 360, 10.0f, 0.01f, 0.0f);
    getME().getMotor().enableControl(0);
    /* The motor might still have momentum from the previous test. */
    Timer.delay(kStartupTime);
  }

  @Ignore("The encoder initial position is not validated so is sometimes not set properly")
  @Test
  public void testSetEncoderInitialPositionWithEnable() {
    // given
    final double encoderValue = 4823;
    // when
    getME().getMotor().enableControl(encoderValue);
    getME().getMotor().disableControl();
    delayTillInCorrectStateWithMessage(Level.FINE, kEncoderSettlingTime, "Encoder value settling",
        new BooleanCheck() {
          @Override
          public boolean getAsBoolean() {
            getME().getMotor().set(getME().getMotor().getPosition());
            return Math.abs(getME().getMotor().getPosition() - encoderValue) < 40;
          }
        });
    // then
    assertEquals(encoderValue, getME().getMotor().getPosition(), 40);
  }

  /**
   * Test if we can set a position and reach that position with PID control on the Jaguar.
   */
  @Test
  public void testEncoderPositionPIDForward() {

    double setpoint = getME().getMotor().getPosition() + 1.0f;

    /* It should get to the setpoint within 10 seconds */
    getME().getMotor().set(setpoint);
    setCANJaguar(kMotorTimeSettling, setpoint);

    assertEquals("CAN Jaguar should have reached setpoint with PID control", setpoint, getME()
        .getMotor().getPosition(), kEncoderPositionTolerance);
  }

  /**
   * Test if we can set a position and reach that position with PID control on the Jaguar.
   */
  @Test
  public void testEncoderPositionPIDReverse() {

    double setpoint = getME().getMotor().getPosition() - 1.0f;

    /* It should get to the setpoint within 10 seconds */
    getME().getMotor().set(setpoint);
    setCANJaguar(kMotorTimeSettling, setpoint);

    assertEquals("CAN Jaguar should have reached setpoint with PID control", setpoint, getME()
        .getMotor().getPosition(), kEncoderPositionTolerance);
  }


}
