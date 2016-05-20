/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;

import static org.junit.Assert.assertTrue;

/**
 * Tests the CAN Jaguar inverted.
 *
 * <p>Created by Patrick Murphy on 3/30/15.
 */
public class CANJaguarInversionTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANJaguarInversionTest.class.getName());
  private static final double m_motorVoltage = 2.0;
  private static final double m_motorPercent = 0.1;
  private static final double m_motorSpeed = 10;
  private static final double m_delayTime = 0.75;
  private static final double m_speedModeDelayTime = 2.0;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void testInvertingVoltageMode() {
    getME().getMotor().setVoltageMode(CANJaguar.kQuadEncoder, 360);
    inversionTest(m_motorVoltage, m_delayTime);
  }

  @Test
  public void testInvertingPercentMode() {
    getME().getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
    inversionTest(m_motorPercent, m_delayTime);
  }

  @Test
  public void testInvertingSpeedMode() {
    getME().getMotor().setSpeedMode(CANJaguar.kQuadEncoder, 360, 0.1, 0.003, 0.01);
    inversionTest(m_motorSpeed, m_speedModeDelayTime);
  }

  /**
   * Runs an inversion test To use set the jaguar to the proper mode(PercentVbus, voltage, speed).
   *
   * @param setPoint  the speed/voltage/percent to set the motor to
   * @param delayTime the amount of time to delay between starting a motor and checking the encoder
   */
  private void inversionTest(double setPoint, double delayTime) {
    final CANJaguar jag = getME().getMotor();
    jag.enableControl();
    jag.setInverted(false);
    jag.set(setPoint);
    Timer.delay(delayTime);
    final double initialSpeed = jag.getSpeed();
    jag.set(0.0);
    jag.setInverted(true);
    jag.set(setPoint);
    Timer.delay(delayTime);
    jag.set(0.0);
    final double finalSpeed = jag.getSpeed();
    assertTrue("Inverting with Positive value does not invert direction",
        Math.signum(initialSpeed) != Math.signum(finalSpeed));
    jag.set(-setPoint);
    Timer.delay(delayTime);
    final double newInitialSpeed = jag.getSpeed();
    jag.set(0.0);
    jag.setInverted(false);
    jag.set(-setPoint);
    Timer.delay(delayTime);
    final double newFinalSpeed = jag.getSpeed();
    jag.set(0.0);
    assertTrue("Inverting with Negative value does not invert direction",
        Math.signum(newInitialSpeed) != Math.signum(newFinalSpeed));
  }
}
