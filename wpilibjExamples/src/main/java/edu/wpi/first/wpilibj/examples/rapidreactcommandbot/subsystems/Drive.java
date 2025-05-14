// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.epilogue.NotLogged;
import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.DriveConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import java.util.function.DoubleSupplier;

@Logged
public class Drive extends SubsystemBase {
  // The motors on the left side of the drive.
  private final PWMSparkMax m_leftLeader = new PWMSparkMax(DriveConstants.LEFT_MOTOR_1_PORT);
  private final PWMSparkMax m_leftFollower = new PWMSparkMax(DriveConstants.LEFT_MOTOR_2_PORT);

  // The motors on the right side of the drive.
  private final PWMSparkMax m_rightLeader = new PWMSparkMax(DriveConstants.RIGHT_MOTOR_1_PORT);
  private final PWMSparkMax m_rightFollower = new PWMSparkMax(DriveConstants.RIGHT_MOTOR_2_PORT);

  // The robot's drive
  @NotLogged // Would duplicate motor data, there's no point sending it twice
  private final DifferentialDrive m_drive =
      new DifferentialDrive(m_leftLeader::set, m_rightLeader::set);

  // The left-side drive encoder
  private final Encoder m_leftEncoder =
      new Encoder(
          DriveConstants.LEFT_ENCODER_PORTS[0],
          DriveConstants.LEFT_ENCODER_PORTS[1],
          DriveConstants.LEFT_ENCODER_REVERSED);

  // The right-side drive encoder
  private final Encoder m_rightEncoder =
      new Encoder(
          DriveConstants.RIGHT_ENCODER_PORTS[0],
          DriveConstants.RIGHT_ENCODER_PORTS[1],
          DriveConstants.RIGHT_ENCODER_REVERSED);

  private final AnalogGyro m_gyro = new AnalogGyro(0);
  private final ProfiledPIDController m_controller =
      new ProfiledPIDController(
          DriveConstants.TURN_P,
          DriveConstants.TURN_I,
          DriveConstants.TURN_D,
          new TrapezoidProfile.Constraints(
              DriveConstants.MAX_TURN_RATE_DEG_PER_S,
              DriveConstants.MAX_TURN_ACCELERATION_DEG_PER_S_SQUARED));
  private final SimpleMotorFeedforward m_feedforward =
      new SimpleMotorFeedforward(DriveConstants.ks, DriveConstants.kv, DriveConstants.ka);

  /** Creates a new Drive subsystem. */
  public Drive() {
    SendableRegistry.addChild(m_drive, m_leftLeader);
    SendableRegistry.addChild(m_drive, m_rightLeader);

    m_leftLeader.addFollower(m_leftFollower);
    m_rightLeader.addFollower(m_rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightLeader.setInverted(true);

    // Sets the distance per pulse for the encoders
    m_leftEncoder.setDistancePerPulse(DriveConstants.ENCODER_DISTANCE_PER_PULSE);
    m_rightEncoder.setDistancePerPulse(DriveConstants.ENCODER_DISTANCE_PER_PULSE);

    // Set the controller to be continuous (because it is an angle controller)
    m_controller.enableContinuousInput(-180, 180);
    // Set the controller tolerance - the delta tolerance ensures the robot is stationary at the
    // setpoint before it is considered as having reached the reference
    m_controller.setTolerance(
        DriveConstants.TURN_TOLERANCE_DEG, DriveConstants.TURN_RATE_TOLERANCE_DEG_PER_S);
  }

  /**
   * Returns a command that drives the robot with arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  public Command arcadeDriveCommand(DoubleSupplier fwd, DoubleSupplier rot) {
    // A split-stick arcade command, with forward/backward controlled by the left
    // hand, and turning controlled by the right.
    return run(() -> m_drive.arcadeDrive(fwd.getAsDouble(), rot.getAsDouble()))
        .withName("arcadeDrive");
  }

  /**
   * Returns a command that drives the robot forward a specified distance at a specified speed.
   *
   * @param distance The distance to drive forward in meters
   * @param speed The fraction of max speed at which to drive
   */
  public Command driveDistanceCommand(double distance, double speed) {
    return runOnce(
            () -> {
              // Reset encoders at the start of the command
              m_leftEncoder.reset();
              m_rightEncoder.reset();
            })
        // Drive forward at specified speed
        .andThen(run(() -> m_drive.arcadeDrive(speed, 0)))
        // End command when we've traveled the specified distance
        .until(
            () -> Math.max(m_leftEncoder.getDistance(), m_rightEncoder.getDistance()) >= distance)
        // Stop the drive when the command ends
        .finallyDo(interrupted -> m_drive.stopMotor());
  }

  /**
   * Returns a command that turns to robot to the specified angle using a motion profile and PID
   * controller.
   *
   * @param angleDeg The angle to turn to
   */
  public Command turnToAngleCommand(double angleDeg) {
    return startRun(
            () -> m_controller.reset(m_gyro.getRotation2d().getDegrees()),
            () ->
                m_drive.arcadeDrive(
                    0,
                    m_controller.calculate(m_gyro.getRotation2d().getDegrees(), angleDeg)
                        // Divide feedforward voltage by battery voltage to normalize it to [-1, 1]
                        + m_feedforward.calculate(m_controller.getSetpoint().velocity)
                            / RobotController.getBatteryVoltage()))
        .until(m_controller::atGoal)
        .finallyDo(() -> m_drive.arcadeDrive(0, 0));
  }
}
