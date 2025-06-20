// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.templates.xrpcommandbased;

import static edu.wpi.first.wpilibj2.command.button.RobotModeTriggers.autonomous;

import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.templates.xrpcommandbased.commands.ExampleCommand;
import edu.wpi.first.wpilibj.templates.xrpcommandbased.subsystems.XRPDrivetrain;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandRobot;
import edu.wpi.first.wpilibj2.command.Commands;
import java.util.Set;

/**
 * This class automatically runs the {@link CommandScheduler} for you. If you change the name of
 * this class or the package after creating this project, you must also update the Main.java file in
 * the project.
 */
public class Robot extends CommandRobot {
  private final XRPDrivetrain m_xrpDrivetrain = new XRPDrivetrain();
  private final SendableChooser<Command> m_autoChooser = new SendableChooser<>();

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    m_autoChooser.setDefaultOption("Example Auto", new ExampleCommand(m_xrpDrivetrain));

    autonomous().whileTrue(Commands.defer(() -> m_autoChooser.getSelected().asProxy(), Set.of()));
  }
}
