// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.encoderhoming;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.discrete.DigitalInput;
import org.wpilib.hardware.motor.Spark;
import org.wpilib.hardware.rotation.Encoder;

/**
 * Encoder mechanism homing snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
public class Robot extends TimedRobot {
  Encoder encoder = new Encoder(0, 1);
  Spark spark = new Spark(0);
  // Limit switch on DIO 2
  DigitalInput limit = new DigitalInput(2);

  /**
   * Runs the motor backwards at half velocity until the limit switch is pressed then turn off the
   * motor and reset the encoder.
   */
  @Override
  public void autonomousPeriodic() {
    if (!limit.get()) {
      spark.setThrottle(-0.5);
    } else {
      spark.setThrottle(0.0);
      encoder.reset();
    }
  }
}
