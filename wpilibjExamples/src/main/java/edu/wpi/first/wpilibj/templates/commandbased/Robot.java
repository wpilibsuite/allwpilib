// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.templates.commandbased;

import static edu.wpi.first.wpilibj2.command.button.RobotModeTriggers.*;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.templates.commandbased.Constants.OperatorConstants;
import edu.wpi.first.wpilibj.templates.commandbased.subsystems.ExampleSubsystem;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

/**
 * This class is where the structure of your robot (subsystems, commands, and trigger mappings)
 * should be declared.
 */
public class Robot extends TimedRobot {
  // The robot's subsystems and commands are defined here...
  private final ExampleSubsystem m_exampleSubsystem = new ExampleSubsystem();

  // Replace with CommandPS4Controller or CommandJoystick if needed
  private final CommandXboxController m_driverController =
      new CommandXboxController(OperatorConstants.kDriverControllerPort);

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public Robot() {
    // Configures the command scheduler to run every tick. This is responsible for polling buttons,
    // adding newly-scheduled commands, running already-scheduled commands, removing finished or
    // interrupted commands, and running subsystem periodic() methods.
    // You can use `addPeriodic`, or override a periodic method in this class, to run any code at
    // your desired period.
    addPeriodic(() -> CommandScheduler.getInstance().run(), kDefaultPeriod);

    // Configure the trigger bindings
    configureBindings();
  }

  /**
   * Use this method to define your trigger->command mappings. Triggers can be created via the
   * {@link Trigger#Trigger(java.util.function.BooleanSupplier)} constructor with an arbitrary
   * predicate, or via the named factories in {@link
   * edu.wpi.first.wpilibj2.command.button.CommandGenericHID}'s subclasses for {@link
   * CommandXboxController Xbox}/{@link edu.wpi.first.wpilibj2.command.button.CommandPS4Controller
   * PS4} controllers or {@link edu.wpi.first.wpilibj2.command.button.CommandJoystick Flight
   * joysticks}. Additionally, you can use trigger factories from {@link
   * edu.wpi.first.wpilibj2.command.button.RobotModeTriggers} to bind commands to a part of the
   * match, such as autonomous.
   */
  private void configureBindings() {
    // Schedule `exampleAuto` when the autonomous period starts and cancelling it when autonomous
    // ends
    autonomous().whileTrue(Autos.exampleAuto(m_exampleSubsystem));

    // Schedule `exampleCommand` when `exampleCondition` changes to `true`
    new Trigger(m_exampleSubsystem::exampleCondition).onTrue(m_exampleSubsystem.exampleCommand());

    // Schedule `anotherCommand` when the Xbox controller's B button is pressed,
    // cancelling on release.
    m_driverController.b().whileTrue(m_exampleSubsystem.anotherCommand());
  }
}
