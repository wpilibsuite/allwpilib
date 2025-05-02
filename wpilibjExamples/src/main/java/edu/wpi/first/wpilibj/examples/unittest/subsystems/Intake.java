// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.unittest.subsystems;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.examples.unittest.Constants.IntakeConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

public class Intake implements AutoCloseable {
  private final PWMSparkMax m_motor;
  private final DoubleSolenoid m_piston;

  public Intake() {
    m_motor = new PWMSparkMax(IntakeConstants.MOTOR_PORT);
    m_piston =
        new DoubleSolenoid(
            0,
            PneumaticsModuleType.CTREPCM,
            IntakeConstants.PISTON_FWD_CHANNEL,
            IntakeConstants.PISTON_REV_CHANNEL);
  }

  public void deploy() {
    m_piston.set(DoubleSolenoid.Value.FORWARD);
  }

  public void retract() {
    m_piston.set(DoubleSolenoid.Value.REVERSE);
    m_motor.set(0); // turn off the motor
  }

  public void activate(double speed) {
    if (isDeployed()) {
      m_motor.set(speed);
    } else { // if piston isn't open, do nothing
      m_motor.set(0);
    }
  }

  public boolean isDeployed() {
    return m_piston.get() == DoubleSolenoid.Value.FORWARD;
  }

  @Override
  public void close() {
    m_piston.close();
    m_motor.close();
  }
}
