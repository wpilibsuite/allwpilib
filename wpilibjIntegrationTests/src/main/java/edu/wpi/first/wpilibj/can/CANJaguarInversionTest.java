/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;
import org.junit.Test;
import static org.junit.Assert.assertTrue;
import java.util.logging.Logger;

/**
 * Created by Patrick Murphy on 3/30/15.
 */
public class CANJaguarInversionTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANJaguarInversionTest.class.getName());
  private static final double motorVoltage = 5.0;
  private static final double motorPercent = 0.5;
  private static final double motorSpeed = 100;
  private static final double delayTime = 0.75;
  private static final double speedModeDelayTime = 2.0;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void testInvertingVoltageMode() {
    getME().getMotor().setVoltageMode(CANJaguar.kQuadEncoder, 360);
    InversionTest(motorVoltage, delayTime);
  }

  @Test
  public void testInvertingPercentMode() {
    getME().getMotor().setPercentMode(CANJaguar.kQuadEncoder, 360);
    InversionTest(motorPercent, delayTime);
  }

  @Test
  public void testInvertingSpeedMode() {
    getME().getMotor().setSpeedMode(CANJaguar.kQuadEncoder, 360, 0.1, 0.003, 0.01);
    InversionTest(motorSpeed, speedModeDelayTime);
  }

  /**
   * Runs an inversion test To use set the jaguar to the proper
   * mode(PercentVbus, voltage, speed)
   *$
   * @param setpoint the speed/voltage/percent to set the motor to
   * @param delayTime the amount of time to delay between starting a motor and
   *        checking the encoder
   */
  private void InversionTest(double setpoint, double delayTime) {
    CANJaguar jag = this.getME().getMotor();
    jag.enableControl();
    jag.setInverted(false);
    jag.set(setpoint);
    Timer.delay(delayTime);
    double initialSpeed = jag.getSpeed();
    jag.set(0.0);
    jag.setInverted(true);
    jag.set(setpoint);
    Timer.delay(delayTime);
    jag.set(0.0);
    double finalSpeed = jag.getSpeed();
    assertTrue("Inverting with Positive value does not invert direction",
        Math.signum(initialSpeed) != Math.signum(finalSpeed));
    jag.set(-setpoint);
    Timer.delay(delayTime);
    initialSpeed = jag.getSpeed();
    jag.set(0.0);
    jag.setInverted(false);
    jag.set(-setpoint);
    Timer.delay(delayTime);
    finalSpeed = jag.getSpeed();
    jag.set(0.0);
    assertTrue("Inverting with Negative value does not invert direction",
        Math.signum(initialSpeed) != Math.signum(finalSpeed));
  }
}
