// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot.subsystems;

import java.util.function.DoubleSupplier;
import org.wpilib.command2.Command;
import org.wpilib.command2.SubsystemBase;
import org.wpilib.drive.DifferentialDrive;
import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.epilogue.Logged;
import org.wpilib.epilogue.NotLogged;
import org.wpilib.examples.rapidreactcommandbot.Constants.DriveConstants;
import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ProfiledPIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.system.RobotController;

@Logged
public class Drive extends SubsystemBase {
  // The motors on the left side of the drive.
  private final PWMSparkMax leftLeader = new PWMSparkMax(DriveConstants.LEFT_MOTOR1_PORT);
  private final PWMSparkMax leftFollower = new PWMSparkMax(DriveConstants.LEFT_MOTOR2_PORT);

  // The motors on the right side of the drive.
  private final PWMSparkMax rightLeader = new PWMSparkMax(DriveConstants.RIGHT_MOTOR1_PORT);
  private final PWMSparkMax rightFollower = new PWMSparkMax(DriveConstants.RIGHT_MOTOR2_PORT);

  // The robot's drive
  @NotLogged // Would duplicate motor data, there's no point sending it twice
  private final DifferentialDrive drive =
      new DifferentialDrive(leftLeader::setThrottle, rightLeader::setThrottle);

  // The left-side drive encoder
  private final Encoder leftEncoder =
      new Encoder(
          DriveConstants.LEFT_ENCODER_PORTS[0],
          DriveConstants.LEFT_ENCODER_PORTS[1],
          DriveConstants.LEFT_ENCODER_REVERSED);

  // The right-side drive encoder
  private final Encoder rightEncoder =
      new Encoder(
          DriveConstants.RIGHT_ENCODER_PORTS[0],
          DriveConstants.RIGHT_ENCODER_PORTS[1],
          DriveConstants.RIGHT_ENCODER_REVERSED);

  private final OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);
  private final ProfiledPIDController controller =
      new ProfiledPIDController(
          DriveConstants.TURN_P,
          DriveConstants.TURN_I,
          DriveConstants.TURN_D,
          new TrapezoidProfile.Constraints(
              DriveConstants.MAX_TURN_RATE_DEG_PER_S,
              DriveConstants.MAX_TURN_ACCELERATION_DEG_PER_S_SQUARED));
  private final SimpleMotorFeedforward feedforward =
      new SimpleMotorFeedforward(DriveConstants.ks, DriveConstants.kv, DriveConstants.ka);

  /** Creates a new Drive subsystem. */
  public Drive() {
    leftLeader.addFollower(leftFollower);
    rightLeader.addFollower(rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.setInverted(true);

    // Sets the distance per pulse for the encoders
    leftEncoder.setDistancePerPulse(DriveConstants.ENCODER_DISTANCE_PER_PULSE);
    rightEncoder.setDistancePerPulse(DriveConstants.ENCODER_DISTANCE_PER_PULSE);

    // Set the controller to be continuous (because it is an angle controller)
    controller.enableContinuousInput(-180, 180);
    // Set the controller tolerance - the delta tolerance ensures the robot is stationary at the
    // setpoint before it is considered as having reached the reference
    controller.setTolerance(
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
    return run(() -> drive.arcadeDrive(fwd.getAsDouble(), rot.getAsDouble()))
        .withName("arcadeDrive");
  }

  /**
   * Returns a command that drives the robot forward a specified distance at a specified velocity.
   *
   * @param distance The distance to drive forward in meters
   * @param velocity The fraction of max velocity at which to drive
   */
  public Command driveDistanceCommand(double distance, double velocity) {
    return runOnce(
            () -> {
              // Reset encoders at the start of the command
              leftEncoder.reset();
              rightEncoder.reset();
            })
        // Drive forward at specified velocity
        .andThen(run(() -> drive.arcadeDrive(velocity, 0)))
        // End command when we've traveled the specified distance
        .until(() -> Math.max(leftEncoder.getDistance(), rightEncoder.getDistance()) >= distance)
        // Stop the drive when the command ends
        .finallyDo(interrupted -> drive.arcadeDrive(0, 0));
  }

  /**
   * Returns a command that turns to robot to the specified angle using a motion profile and PID
   * controller.
   *
   * @param angleDeg The angle to turn to
   */
  public Command turnToAngleCommand(double angleDeg) {
    return startRun(
            () -> controller.reset(imu.getRotation2d().getDegrees()),
            () ->
                drive.arcadeDrive(
                    0,
                    controller.calculate(imu.getRotation2d().getDegrees(), angleDeg)
                        // Divide feedforward voltage by battery voltage to normalize it to [-1, 1]
                        + feedforward.calculate(controller.getSetpoint().velocity)
                            / RobotController.getBatteryVoltage()))
        .until(controller::atGoal)
        .finallyDo(() -> drive.arcadeDrive(0, 0));
  }
}
