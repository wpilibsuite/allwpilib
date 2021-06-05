// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PneumaticsControlModule;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.DoubleSolenoid.Value;

@SuppressWarnings("all")
public class MyRobot extends TimedRobot {
  PneumaticsControlModule pcm = new PneumaticsControlModule();
  Solenoid solenoid = new Solenoid(pcm, 1);
  DoubleSolenoid doubleSolenoid = new DoubleSolenoid(pcm, 2, 3);
  XboxController xbox = new XboxController(0);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  @Override
  public void robotInit() {}

  /** This function is run once each time the robot enters autonomous mode */
  @Override
  public void autonomousInit() {}

  /** This function is called periodically during autonomous */
  @Override
  public void autonomousPeriodic() {}

  /** This function is called once each time the robot enters tele-operated mode */
  @Override
  public void teleopInit() {}

  /** This function is called periodically during operator control */
  @Override
  public void teleopPeriodic() {
    solenoid.set(xbox.getAButton());
    if (xbox.getBButton()) {
      doubleSolenoid.set(Value.kForward);
    } else if (xbox.getYButton()) {
      doubleSolenoid.set(Value.kReverse);
    } else {
      doubleSolenoid.set(Value.kOff);
    }
  }

  /** This function is called periodically during test mode */
  @Override
  public void testPeriodic() {}

  /** This function is called periodically during all modes */
  @Override
  public void robotPeriodic() {}
}
