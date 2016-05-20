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

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

import static org.junit.Assert.assertEquals;

/**
 * Tests the CAN Motor Controller in Current Quad Encoder mode.
 *
 * @author jonathanleitschuh
 */
public class CANCurrentQuadEncoderModeTest extends AbstractCANTest {
  private static Logger logger = Logger.getLogger(CANCurrentQuadEncoderModeTest.class.getName());
  private static final double kStoppedValue = 0;
  private static final double kRunningValue = 3.0;

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
  public void setUp() throws Exception {
    getME().getMotor().setCurrentMode(CANJaguar.kQuadEncoder, 360, 10.0, 4.0, 1.0);
    getME().getMotor().enableControl();
    getME().getMotor().set(0.0f);
    /* The motor might still have momentum from the previous test. */
    Timer.delay(kStartupTime);
  }


  @Ignore
  @Test
  public void testDriveToCurrentPositive() {
    double setpoint = 1.6f;

    /* It should get to the setpoint within 10 seconds */
    for (int i = 0; i < 10; i++) {
      setCANJaguar(1.0, setpoint);

      if (Math.abs(getME().getMotor().getOutputCurrent() - setpoint) <= kCurrentTolerance) {
        break;
      }
    }

    assertEquals("The desired output current was not reached", setpoint, getME().getMotor()
        .getOutputCurrent(), kCurrentTolerance);
  }

  @Ignore
  @Test
  public void testDriveToCurrentNegative() {
    double setpoint = -1.6f;

    /* It should get to the setpoint within 10 seconds */
    for (int i = 0; i < 10; i++) {
      setCANJaguar(1.0, setpoint);

      if (Math.abs(getME().getMotor().getOutputCurrent() - Math.abs(setpoint))
          <= kCurrentTolerance) {
        break;
      }
    }

    assertEquals("The desired output current was not reached", Math.abs(setpoint), getME()
        .getMotor().getOutputCurrent(), kCurrentTolerance);
  }

}
