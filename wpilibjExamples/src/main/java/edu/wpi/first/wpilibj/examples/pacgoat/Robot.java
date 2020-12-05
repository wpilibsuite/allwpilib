// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat;

import edu.wpi.first.wpilibj.PneumaticsControlModule;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.DriveAndShootAutonomous;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.DriveForward;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.DriveTrain;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pneumatics;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Shooter;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This is the main class for running the PacGoat code.
 *
 * <p>The VM is configured to automatically run this class, and to call the functions corresponding
 * to each mode, as described in the TimedRobot documentation. If you change the name of this class
 * or the package after creating this project, you must also update the manifest file in the
 * resource directory.
 */
public class Robot extends TimedRobot {
  Command m_autonomousCommand;
  public static OI oi;

  public static PneumaticsControlModule pneumaticsModule = new PneumaticsControlModule(1);

  // Initialize the subsystems
  public static DriveTrain drivetrain = new DriveTrain();
  public static Collector collector = new Collector();
  public static Shooter shooter = new Shooter();
  public static Pneumatics pneumatics = new Pneumatics();
  public static Pivot pivot = new Pivot();

  public SendableChooser<Command> m_autoChooser;

  // This function is run when the robot is first started up and should be
  // used for any initialization code.
  @Override
  public void robotInit() {
    SmartDashboard.putData(drivetrain);
    SmartDashboard.putData(collector);
    SmartDashboard.putData(shooter);
    SmartDashboard.putData(pneumatics);
    SmartDashboard.putData(pivot);

    // This MUST be here. If the OI creates Commands (which it very likely
    // will), constructing it during the construction of CommandBase (from
    // which commands extend), subsystems are not guaranteed to be
    // yet. Thus, their requires() statements may grab null pointers. Bad
    // news. Don't move it.
    oi = new OI();

    // instantiate the command used for the autonomous period
    m_autoChooser = new SendableChooser<>("Auto Modes");
    m_autoChooser.setDefaultOption("Drive and Shoot", new DriveAndShootAutonomous());
    m_autoChooser.addOption("Drive Forward", new DriveForward());
  }

  @Override
  public void autonomousInit() {
    m_autonomousCommand = m_autoChooser.getSelected();
    m_autonomousCommand.start();
  }

  // This function is called periodically during autonomous
  @Override
  public void autonomousPeriodic() {
    Scheduler.getInstance().run();
    log();
  }

  @Override
  public void teleopInit() {
    // This makes sure that the autonomous stops running when
    // teleop starts running. If you want the autonomous to
    // continue until interrupted by another command, remove
    // this line or comment it out.
    if (m_autonomousCommand != null) {
      m_autonomousCommand.cancel();
    }
  }

  // This function is called periodically during operator control
  @Override
  public void teleopPeriodic() {
    Scheduler.getInstance().run();
    log();
  }

  // This function called periodically during test mode
  @Override
  public void testPeriodic() {}

  @Override
  public void disabledInit() {
    Robot.shooter.unlatch();
  }

  // This function is called periodically while disabled
  @Override
  public void disabledPeriodic() {
    log();
  }

  /** Log interesting values to the SmartDashboard. */
  private void log() {
    Robot.pneumatics.writePressure();
    SmartDashboard.putNumber("Pivot Pot Value", Robot.pivot.getAngle());
    SmartDashboard.putNumber("Left Distance", drivetrain.getLeftEncoder().getDistance());
    SmartDashboard.putNumber("Right Distance", drivetrain.getRightEncoder().getDistance());
  }
}
