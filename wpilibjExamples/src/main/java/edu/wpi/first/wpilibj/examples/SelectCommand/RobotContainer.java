package edu.wpi.first.wpilibj.examples.SelectCommand;

import java.util.Map;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.experimental.command.Command;
import edu.wpi.first.wpilibj.experimental.command.PrintCommand;
import edu.wpi.first.wpilibj.experimental.command.RunCommand;
import edu.wpi.first.wpilibj.experimental.command.SelectCommand;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;

import static edu.wpi.first.wpilibj.XboxController.Button;
import static edu.wpi.first.wpilibj.templates.commandbased.Constants.OIConstants.kDriverControllerPort;
import static java.util.Map.entry;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {

  // The enum used as keys for selecting the command to run.
  private enum CommandSelector {
    one, two, three
  }

  // An example selector method for the SelectCommand.  Returns the selector that will select
  // which command to run.  Can base this choice on logical conditions evaluated at runtime.
  private CommandSelector select() {
    return CommandSelector.one;
  }

  // An example SelectCommand.  Will select from the three commands based on the value returned
  // by the selector method at runtime.  Note that SelectCommand takes a generic type, so the
  // selector does not have to be an enum; it could be any desired type (string, integer,
  // boolean, double...)
  private Command exampleSelectCommand =
      new SelectCommand<CommandSelector>(
          // Maps selector values to commands
          Map.ofEntries(
              entry(CommandSelector.one, new PrintCommand("Command one was selected!")),
              entry(CommandSelector.two, new PrintCommand("Command two was selected!")),
              entry(CommandSelector.three, new PrintCommand("Command three was selected!"))
          ),
          this::select
      );

  public RobotContainer() {
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
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return exampleSelectCommand;
  }
}
