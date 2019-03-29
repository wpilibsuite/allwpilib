package edu.wpi.first.wpilibj.examples.frisbeebot;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.examples.frisbeebot.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.frisbeebot.subsystems.ShooterSubsystem;
import edu.wpi.first.wpilibj.experimental.command.*;

import static edu.wpi.first.wpilibj.XboxController.Button;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.AutoConstants.*;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.OIConstants.*;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {

  // The robot's subsystems
  private DriveSubsystem m_robotDrive = new DriveSubsystem();
  private ShooterSubsystem m_shooter = new ShooterSubsystem();

  // A simple autonomous routine that shoots the loaded frisbees
  private Command m_autoCommand =
      // Start the command by spinning up the shooter...
      new InstantCommand(() -> m_shooter.setEnabled(true), m_shooter)
          .andThen(
              // Wait until the shooter is at speed before feeding the frisbees
              new WaitUntilCommand(m_shooter::atReference),
              // Start running the feeder
              new InstantCommand(m_shooter::runFeeder, m_shooter),
              // Shoot for the specified time
              new WaitCommand(kAutoShootTimeSeconds)
          )
          // Add a timeout (will end the command if, for instance, the shooter never gets up to
          // speed)
          .withTimeout(kAutoTimeoutSeconds)
          // When the command ends, turn off the shooter and the feeder
          .whenFinished(() -> {
            m_shooter.setEnabled(false);
            m_shooter.stopFeeder();
          });

  // The driver's controller
  XboxController driverController = new XboxController(kDriverControllerPort);

  public RobotContainer() {
    // Configure the button bindings
    configureButtonBindings();

    // Configure default commands
    // Set the default drive command to split-stick arcade drive
    m_robotDrive.setDefaultCommand(
        // A split-stick arcade command, with forward/backward controlled by the left
        // hand, and turning controlled by the right.
        new RunCommand(() -> m_robotDrive.arcadeDrive(
            driverController.getY(GenericHID.Hand.kLeft),
            driverController.getX(GenericHID.Hand.kRight)),
            m_robotDrive)
    );

  }

  /**
   * Use this method to define your button->command mappings.  Buttons can be created by
   * instantiating a {@link GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link XboxController}), and then calling {@link
   * GenericHID#getButton(int)}.
   */
  private void configureButtonBindings() {
    // Spin up the shooter when the 'A' button is pressed
    driverController.getButton(Button.kA.value)
        .whenPressed(new InstantCommand(() -> m_shooter.setEnabled(true), m_shooter));

    // Turn off the shooter when the 'B' button is pressed
    driverController.getButton(Button.kB.value)
        .whenPressed(new InstantCommand(() -> m_shooter.setEnabled(false), m_shooter));

    // Run the feeder when the 'X' button is held, but only if the shooter is at speed
    driverController.getButton(Button.kX.value)
        .whenPressed(new ConditionalCommand(
            // Run the feeder
            new InstantCommand(m_shooter::runFeeder, m_shooter),
            // Do nothing
            new InstantCommand(),
            // Determine which of the above to do based on whether the shooter has reached the
            // desired speed
            m_shooter::atReference))
        .whenReleased(new InstantCommand(m_shooter::stopFeeder, m_shooter));

    // Drive at half speed when the bumper is held
    driverController.getButton(Button.kBumperRight.value)
        .whenPressed(() -> m_robotDrive.setMaxOutput(.5))
        .whenReleased(() -> m_robotDrive.setMaxOutput(1));
  }


  /**
   * Use this to pass the autonomous command to the main {@link Robot} class.
   *
   * @return the command to run in autonomous
   */
  public Command getAutonomousCommand() {
    return m_autoCommand;
  }
}
