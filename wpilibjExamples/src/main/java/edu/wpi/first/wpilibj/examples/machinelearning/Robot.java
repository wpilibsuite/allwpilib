/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.machinelearning;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj2.command.button.Button;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;

public class Robot extends TimedRobot {
  private VisionSubsystem visionSubsystem = new VisionSubsystem();
  private DriveSubsystem driveSubsystem = new DriveSubsystem();
  private final XboxController m_controller = new XboxController(0);


  @Override
  public void robotInit() {
    super.robotInit();
    final Button a = new JoystickButton(m_controller, XboxController.Button.kA.value);
    a.whenPressed(new TurnToBallCommand(driveSubsystem, visionSubsystem));
  }
}
