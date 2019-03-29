package edu.wpi.first.wpilibj.templates.commandbased;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.experimental.command.Command;
import edu.wpi.first.wpilibj.experimental.command.RunCommand;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.templates.commandbased.commands.ExampleCommand;
import edu.wpi.first.wpilibj.templates.commandbased.subsystems.ExampleSubsystem;

import static edu.wpi.first.wpilibj.XboxController.Button;
import static edu.wpi.first.wpilibj.templates.commandbased.Constants.OIConstants.*;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {

  // An example robot subsystem.  Keeping subsystem fields private prevents you from accidentally
  // interacting with them from elsewhere in the code, which can cause unpredictable and
  // hard-to-diagnose behavior.
  private ExampleSubsystem exampleSubsystem = new ExampleSubsystem();

  // A chooser for autonomous commands.
  SendableChooser<Command> m_chooser = new SendableChooser<>();

  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();

    // Add commands to the autonomous command chooser
    m_chooser.addOption("Example Auto 1",
        new RunCommand(exampleSubsystem::exampleMethod, exampleSubsystem).withTimeout(15));
    m_chooser.addOption("Example Auto 2", new ExampleCommand(exampleSubsystem));

    // Put the chooser on the dashboard
    Shuffleboard.getTab("Autonomous").add(m_chooser);
  }

  /**
   * Use this method to define your button->command mappings.  Buttons can be created by
   * instantiating a {@link edu.wpi.first.wpilibj.GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link edu.wpi.first.wpilibj.XboxController}), and then
   * calling {@link edu.wpi.first.wpilibj.GenericHID#getButton(int)}.
   */
  private void configureButtonBindings() {
    // Here's an example HID (in this case, an Xbox controller):
    XboxController driverController = new XboxController(kDriverControllerPort);
    // Binding a command to run when the 'A' button is held:
    driverController.getButton(Button.kA.value)
        .whenHeld(new RunCommand(exampleSubsystem::exampleMethod, exampleSubsystem));
    // Binding a command to run when both the 'X' and 'Y' buttons are pressed:
    driverController.getButton(Button.kX.value).and(driverController.getButton(Button.kY.value))
        .whenActive(new ExampleCommand(exampleSubsystem));
  }

  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return m_chooser.getSelected();
  }
}
