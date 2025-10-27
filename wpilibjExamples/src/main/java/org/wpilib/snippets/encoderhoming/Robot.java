// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.encoderhoming;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.Spark;

/**
 * Encoder mechanism homing snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
public class Robot extends TimedRobot {
  Encoder m_encoder = new Encoder(0, 1);
  Spark m_spark = new Spark(0);
  // Limit switch on DIO 2
  DigitalInput m_limit = new DigitalInput(2);

  /**
   * Runs the motor backwards at half speed until the limit switch is pressed then turn off the
   * motor and reset the encoder.
   */
  @Override
  public void autonomousPeriodic() {
    if (!m_limit.get()) {
      m_spark.set(-0.5);
    } else {
      m_spark.set(0.0);
      m_encoder.reset();
    }
  }
}
