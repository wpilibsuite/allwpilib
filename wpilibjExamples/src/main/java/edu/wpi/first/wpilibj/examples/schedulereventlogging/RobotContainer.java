package edu.wpi.first.wpilibj.examples.schedulereventlogging;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.experimental.command.*;
import edu.wpi.first.wpilibj.shuffleboard.EventImportance;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;

import static edu.wpi.first.wpilibj.examples.schedulereventlogging.Constants.OIConstants.*;
import static edu.wpi.first.wpilibj.XboxController.*;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {

  // The driver's controller
  private XboxController m_driverController = new XboxController(kDriverControllerPort);

  // A few commands that do nothing, but will demonstrate the scheduler functionality
  private SendableCommandBase m_instantCommand1 = new InstantCommand();
  private SendableCommandBase m_instantCommand2 = new InstantCommand();
  private SendableCommandBase m_waitCommand = new WaitCommand(5);

  public RobotContainer() {
    // Set names of commands
    m_instantCommand1.setName("Instant Command 1");
    m_instantCommand2.setName("Instant Command 2");
    m_waitCommand.setName("Wait 5 Seconds Command");

    // Set the scheduler to log Shuffleboard events for command initialize, interrupt, finish
    CommandScheduler.getInstance().onCommandInitialize(command -> Shuffleboard.addEventMarker(
        "Command initialized", command.getName(), EventImportance.kNormal));
    CommandScheduler.getInstance().onCommandInterrupted(command -> Shuffleboard.addEventMarker(
        "Command interrupted", command.getName(), EventImportance.kNormal));
    CommandScheduler.getInstance().onCommandFinished(command -> Shuffleboard.addEventMarker(
        "Command finished", command.getName(), EventImportance.kNormal));

    // Configure the button bindings
    configureButtonBindings();
  }

  /**
   * Use this method to define your button->command mappings.  Buttons can be created by
   * instantiating a {@link edu.wpi.first.wpilibj.GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link XboxController}), and then
   * calling {@link edu.wpi.first.wpilibj.GenericHID#getButton(int)}.
   */
  private void configureButtonBindings() {
    // Run instant command 1 when the 'A' button is pressed
    m_driverController.getButton(Button.kA.value).whenPressed(m_instantCommand1);
    // Run instant command 2 when the 'X' button is pressed
    m_driverController.getButton(Button.kX.value).whenPressed(m_instantCommand2);
    // Run instant command 3 when the 'Y' button is held; release early to interrupt
    m_driverController.getButton(Button.kY.value).whenHeld(m_waitCommand);

  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return new InstantCommand();
  }
}
