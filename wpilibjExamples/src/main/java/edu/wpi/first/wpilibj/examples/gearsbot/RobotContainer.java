// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Autos;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Placement;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Drivetrain;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems and commands are defined here...
  private final Drivetrain m_drivetrain = new Drivetrain();
  private final Elevator m_elevator = new Elevator();
  private final Wrist m_wrist = new Wrist();
  private final Claw m_claw = new Claw();

  private final CommandXboxController m_controller = new CommandXboxController(0);

  private final Autos m_autos = new Autos(m_drivetrain, m_elevator, m_wrist, m_claw);
  private final Placement m_placement = new Placement(m_claw, m_wrist, m_elevator);

  /** The container for the robot. Contains subsystems, OI devices, and commands. */
  public RobotContainer() {
    // Put Some buttons on the SmartDashboard
    SmartDashboard.putData("Elevator Bottom", m_elevator.goTo(() -> 0));
    SmartDashboard.putData("Elevator Top", m_elevator.goTo(() -> 0.25));

    SmartDashboard.putData("Wrist Horizontal", m_wrist.goTo(() -> 0));
    SmartDashboard.putData("Raise Wrist", m_wrist.goTo(() -> -45));

    SmartDashboard.putData("Open Claw", m_claw.open());
    SmartDashboard.putData("Close Claw", m_claw.close());

    SmartDashboard.putData("Deliver Soda", m_autos.deliverSoda());

    // Assign default commands
    m_drivetrain.setDefaultCommand(
        m_drivetrain
            .tankDrive(() -> -m_controller.getLeftY(), () -> -m_controller.getRightY())
            .withName("teleop driving"));

    // Show what command your subsystem is running on the SmartDashboard
    SmartDashboard.putData(m_drivetrain);
    SmartDashboard.putData(m_elevator);
    SmartDashboard.putData(m_wrist);
    SmartDashboard.putData(m_claw);

    // Configure the button bindings
    configureButtonBindings();
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link edu.wpi.first.wpilibj.GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link XboxController}), and then passing it to a {@link
   * edu.wpi.first.wpilibj2.command.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Connect the buttons to commands
    m_controller.povUp().onTrue(m_elevator.goTo(() -> 0.25));
    m_controller.povDown().onTrue(m_elevator.goTo(() -> 0.0));
    m_controller.povRight().onTrue(m_claw.close());
    m_controller.povLeft().onTrue(m_claw.open());

    m_controller.rightBumper().onTrue(m_placement.prepareToPickup());
    m_controller.rightTrigger().onTrue(m_placement.pickup());
    m_controller.leftBumper().onTrue(m_placement.place());
    m_controller.leftTrigger().onTrue(m_autos.deliverSoda());
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return m_autos.deliverSoda();
  }
}
