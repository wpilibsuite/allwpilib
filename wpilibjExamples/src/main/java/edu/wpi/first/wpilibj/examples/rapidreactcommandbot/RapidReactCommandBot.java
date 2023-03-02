// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot;

import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.HAL.SimPeriodicAfterCallback;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.DriveSim;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.ShooterSim;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Drive;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Intake;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Pneumatics;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Shooter;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Storage;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.ProxyCommand;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, the structure of the robot (including subsystems, commands, and trigger
 * bindings) should be set up decoratively here, as opposed to iteratively handling logic in the
 * {@link TimedRobot} periodic methods.
 */
public class RapidReactCommandBot extends TimedRobot implements AutoCloseable {
  // The robot's subsystems
  private final Drive m_drive = new Drive();
  private final Intake m_intake = new Intake();
  private final Storage m_storage = new Storage();
  private final Shooter m_shooter = new Shooter();
  private final Pneumatics m_pneumatics = new Pneumatics();

  // The driver's controller
  CommandXboxController m_driverController =
      new CommandXboxController(OIConstants.kDriverControllerPort);

  /**
   * Use this method to define bindings between conditions and commands. These are useful for
   * automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called during {@link #robotInit()}.
   *
   * <p>Event binding methods are available on the {@link Trigger} class.
   */
  public void configureBindings() {
    // Automatically run the storage motor whenever the ball storage is not full,
    // and turn it off whenever it fills. Uses subsystem-hosted trigger to
    // improve readability and make inter-subsystem communication easier.
    m_storage.hasCargo.whileFalse(m_storage.runCommand());

    // Automatically disable and retract the intake whenever the ball storage is full.
    m_storage.hasCargo.onTrue(m_intake.retractCommand());

    // Control the drive with split-stick arcade controls
    m_drive.setDefaultCommand(
        m_drive.arcadeDriveCommand(
            () -> -m_driverController.getLeftY(), () -> -m_driverController.getRightX()));

    // Deploy the intake with the X button
    m_driverController.x().onTrue(m_intake.intakeCommand());
    // Retract the intake with the Y button
    m_driverController.y().onTrue(m_intake.retractCommand());

    // Fire the shooter with the A button
    m_driverController
        .a()
        .onTrue(
            parallel(
                    m_shooter.shootCommand(ShooterConstants.kShooterTargetRPS),
                    m_storage.runCommand())
                // Since we composed this inline we should give it a name
                .withName("Shoot"));

    // Toggle compressor with the Start button
    m_driverController.start().toggleOnTrue(m_pneumatics.disableCompressorCommand());

    // Bind a command to run during auto.
    // whileTrue will cancel the command when auto ends;
    // if this behavior is not desired, change whileTrue to onTrue.
    new Trigger(DriverStation::isAutonomous)
        .whileTrue(Commands.deferred(() -> this.getAutonomousCommand().asProxy()));
  }

  /**
   * Use this to define the command that runs during autonomous.
   *
   * <p>Scheduled during {@link #autonomousInit()}.
   */
  public Command getAutonomousCommand() {
    // Drive forward for 2 meters at half speed with a 3 second timeout
    return m_drive
        .driveDistanceCommand(AutoConstants.kDriveDistanceMeters, AutoConstants.kDriveSpeed)
        .withTimeout(AutoConstants.kTimeoutSeconds);
  }

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code. In this case, we set up all bindings and periodic tasks.
   */
  @Override
  public void robotInit() {
    // Add the Scheduler as a periodic task. This is responsible for polling buttons, adding
    // newly-scheduled commands, running already-scheduled commands, removing finished or
    // interrupted commands, and running subsystem periodic() methods. This is required for anything
    // in the Command-based framework to work.
    addPeriodic(CommandScheduler.getInstance()::run, getPeriod());

    // Configure default commands and condition bindings on robot startup
    configureBindings();
  }

  /* ******************* Simulation ******************* */

  // Simulation controllers
  private ShooterSim m_shooterSim;
  private DriveSim m_driveSim;
  // We need these as fields here so they don't get destroyed and cancelled.
  private SimPeriodicAfterCallback m_shooterSimCallback;
  private SimPeriodicAfterCallback m_driveSimCallback;

  /** Setup simulation. */
  @Override
  public void simulationInit() {
    // Init sim controllers
    m_shooterSim = new ShooterSim();
    m_driveSim = new DriveSim();
    // Register them to be run periodically in sim.
    m_shooterSimCallback = HAL.registerSimPeriodicAfterCallback(m_shooterSim::simulationPeriodic);
    m_driveSimCallback = HAL.registerSimPeriodicAfterCallback(m_driveSim::simulationPeriodic);
  }

  @Override
  public void close() {
    m_drive.close();
    m_intake.close();
    m_storage.close();
    m_shooter.close();
    m_pneumatics.close();
  }
}
