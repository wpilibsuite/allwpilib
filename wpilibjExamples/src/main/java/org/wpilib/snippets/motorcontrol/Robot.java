// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.motorcontrol;

import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.smartdashboard.SmartDashboard;

/**
 * This sample program shows how to control a motor using a joystick. In the operator control part
 * of the program, the joystick is read and the value is written to the motor.
 *
 * <p>Joystick analog values range from -1 to 1 and motor controller inputs also range from -1 to 1
 * making it easy to work together.
 *
 * <p>In addition, the encoder value of an encoder connected to ports 0 and 1 is consistently sent
 * to the Dashboard.
 *
 * <p>Finally, short code snippets show how to invert the motor direction and how to use the motor
 * safety for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/wpi-drive-classes.html
 */
public class Robot extends TimedRobot {
  private static final int kMotorPort = 0;
  private static final int kJoystickPort = 0;
  private static final int kEncoderPortA = 0;
  private static final int kEncoderPortB = 1;

  private final PWMSparkMax motor;
  private final Joystick joystick;
  private final Encoder encoder;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    motor = new PWMSparkMax(kMotorPort);
    joystick = new Joystick(kJoystickPort);
    encoder = new Encoder(kEncoderPortA, kEncoderPortB);
    // Use SetDistancePerPulse to set the multiplier for GetDistance
    // This is set up assuming a 6 inch wheel with a 360 CPR encoder.
    encoder.setDistancePerPulse((Math.PI * 6) / 360.0);

    // show motor inversion
    motor.setInverted(true);

    // show motor safety features
    motor.setSafetyEnabled(true);
    motor.setSafetyEnabled(false);
    motor.setExpiration(0.1);
    motor.feed();
  }

  /*
   * The RobotPeriodic function is called every control packet no matter the
   * robot mode.
   */
  @Override
  public void robotPeriodic() {
    SmartDashboard.putNumber("Encoder", encoder.getDistance());
  }

  /** The teleop periodic function is called every control packet in teleop. */
  @Override
  public void teleopPeriodic() {
    motor.setThrottle(joystick.getY());
  }
}
