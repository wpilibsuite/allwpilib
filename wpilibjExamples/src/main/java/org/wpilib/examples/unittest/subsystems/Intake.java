// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.unittest.subsystems;

import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.examples.unittest.Constants.IntakeConstants;
import org.wpilib.hardware.bus.CANBus;
import org.wpilib.hardware.pneumatic.DoubleSolenoid;
import org.wpilib.hardware.pneumatic.PneumaticsModuleType;

public class Intake implements AutoCloseable {
  private final PWMSparkMax motor;
  private final DoubleSolenoid piston;

  public Intake() {
    motor = new PWMSparkMax(IntakeConstants.MOTOR_PORT);
    piston =
        new DoubleSolenoid(
            CANBus.CAN_S0,
            PneumaticsModuleType.CTRE_PCM,
            IntakeConstants.PISTON_FWD_CHANNEL,
            IntakeConstants.PISTON_REV_CHANNEL);
  }

  public void deploy() {
    piston.set(DoubleSolenoid.Value.FORWARD);
  }

  public void retract() {
    piston.set(DoubleSolenoid.Value.REVERSE);
    motor.setThrottle(0); // turn off the motor
  }

  public void activate(double velocity) {
    if (isDeployed()) {
      motor.setThrottle(velocity);
    } else { // if piston isn't open, do nothing
      motor.setThrottle(0);
    }
  }

  public boolean isDeployed() {
    return piston.get() == DoubleSolenoid.Value.FORWARD;
  }

  @Override
  public void close() {
    piston.close();
    motor.close();
  }
}
