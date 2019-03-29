package edu.wpi.first.wpilibj.examples.hatchbot;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.examples.hatchbot.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbot.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.experimental.command.Command;
import edu.wpi.first.wpilibj.experimental.command.InstantCommand;
import edu.wpi.first.wpilibj.experimental.command.RunCommand;
import edu.wpi.first.wpilibj.experimental.command.StartEndCommand;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;

import static edu.wpi.first.wpilibj.examples.hatchbot.Constants.*;
import static edu.wpi.first.wpilibj.XboxController.Button;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {

  // The robot's subsystems
  private DriveSubsystem m_robotDrive = new DriveSubsystem();
  private HatchSubsystem m_hatchSubsystem = new HatchSubsystem();

  // The autonomous routines

  // A simple auto routine that drives forward a specified distance, and then stops.
  private final Command m_simpleAuto =
      new StartEndCommand(
          // Start driving forward at the start of the command
          () -> m_robotDrive.arcadeDrive(.5, 0),
          // Stop driving at the end of the command
          () -> m_robotDrive.arcadeDrive(0, 0),
          // Requires the drive subsystem
          m_robotDrive
      )
          // Reset the encoders before starting
          .beforeStarting(m_robotDrive::resetEncoders)
          // End the command when the robot's driven distance exceeds the desired value
          .interruptOn(() -> m_robotDrive.getAverangeEncoderDistance() >= kAutoDriveDistanceInches);

  // A complex auto routine that drives forward, drops a hatch, and then drives backward.
  private final Command m_complexAuto =
      new StartEndCommand(
          // Start driving forward at the start of the command
          () -> m_robotDrive.arcadeDrive(.5, 0),
          // Stop driving at the end of the command
          () -> m_robotDrive.arcadeDrive(0, 0), m_robotDrive
      )
          // End the command when the robot's driven distance exceeds the desired value
          .interruptOn(() -> m_robotDrive.getAverangeEncoderDistance() >= kAutoDriveDistanceInches)
          // Reset the encoders before starting
          .beforeStarting(m_robotDrive::resetEncoders)
          // Then...
          .andThen(
              // Release the hatch
              new InstantCommand(m_hatchSubsystem::releaseHatch, m_hatchSubsystem),
              // And drive backwards for the specified distance
              new StartEndCommand(
                  () -> m_robotDrive.arcadeDrive(-.5, 0),
                  () -> m_robotDrive.arcadeDrive(0, 0),
                  m_robotDrive
              )
                  .beforeStarting(m_robotDrive::resetEncoders)
                  .interruptOn(
                      () -> m_robotDrive.getAverangeEncoderDistance() <= -kAutoBackupDistanceInches)
          );

  // A chooser for autonomous commands
  SendableChooser<Command> m_chooser = new SendableChooser<>();

  // The driver's controller
  XboxController driverController = new XboxController(Constants.kDriverControllerPort);

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

    // Add commands to the autonomous command chooser
    m_chooser.addOption("Simple Auto", m_simpleAuto);
    m_chooser.addOption("Complex Auto", m_complexAuto);

    // Put the chooser on the dashboard
    Shuffleboard.getTab("Autonomous").add(m_chooser);
  }

  /**
   * Use this method to define your button->command mappings.  Buttons can be created by
   * instantiating a {@link edu.wpi.first.wpilibj.GenericHID} or one of its subclasses ({@link
   * edu.wpi.first.wpilibj.Joystick} or {@link XboxController}), and then calling {@link
   * edu.wpi.first.wpilibj.GenericHID#getButton(int)}.
   */
  private void configureButtonBindings() {
    // Grab the hatch when the 'A' button is pressed.
    driverController.getButton(Button.kA.value)
        .whenPressed(new InstantCommand(m_hatchSubsystem::grabHatch, m_hatchSubsystem));
    // Release the hatch when the 'B' button is pressed.
    driverController.getButton(Button.kB.value)
        .whenPressed(new InstantCommand(m_hatchSubsystem::releaseHatch, m_hatchSubsystem));
    // While holding the shoulder button, drive at half speed
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
    return m_chooser.getSelected();
  }
}
