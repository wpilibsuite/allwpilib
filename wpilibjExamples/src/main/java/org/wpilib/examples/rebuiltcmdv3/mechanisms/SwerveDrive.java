// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.Radians;
import static org.wpilib.units.Units.RadiansPerSecond;

import java.util.function.Function;
import java.util.function.Supplier;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.command3.button.CommandGamepad;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.DriveConstants;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;
import org.wpilib.examples.rebuiltcmdv3.stubs.PathFollower;
import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.SwerveDriveKinematics;
import org.wpilib.math.kinematics.SwerveModulePosition;
import org.wpilib.math.kinematics.SwerveModuleVelocity;
import org.wpilib.units.measure.Angle;
import org.wpilib.units.measure.AngularVelocity;

@Logged
public class SwerveDrive implements Mechanism {
  public final SwerveDriveKinematics kinematics = DriveConstants.KINEMATICS;
  private final OnboardIMU gyro = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);
  private final SwerveModule frontLeft =
      new SwerveModule(
          new ExampleSmartMotorController(DriveConstants.FRONT_LEFT_DRIVE_ID),
          new ExampleSmartMotorController(DriveConstants.FRONT_LEFT_TURN_ID));
  private final SwerveModule frontRight =
      new SwerveModule(
          new ExampleSmartMotorController(DriveConstants.FRONT_RIGHT_DRIVE_ID),
          new ExampleSmartMotorController(DriveConstants.FRONT_RIGHT_TURN_ID));
  private final SwerveModule rearLeft =
      new SwerveModule(
          new ExampleSmartMotorController(DriveConstants.REAR_LEFT_DRIVE_ID),
          new ExampleSmartMotorController(DriveConstants.REAR_LEFT_TURN_ID));
  private final SwerveModule rearRight =
      new SwerveModule(
          new ExampleSmartMotorController(DriveConstants.REAR_RIGHT_DRIVE_ID),
          new ExampleSmartMotorController(DriveConstants.REAR_RIGHT_TURN_ID));

  // PID controllers for XY field position and heading
  // These accept position (meters for XY, radians for heading) and output velocities
  // (meters per second for XY, radians per second for heading)
  private final PIDController xController = new PIDController(0.05, 0, 0);
  private final PIDController yController = new PIDController(0.05, 0, 0);
  private final PIDController headingController = new PIDController(0.05, 0, 0);

  public SwerveDrive() {
    // Enable continuous input on the heading PID controller to handle wraparound.
    headingController.enableContinuousInput(-Math.PI, Math.PI);
  }

  /**
   * Gets the heading of the drivebase. Used for localization.
   *
   * @return The heading of the drivebase.
   */
  public Rotation2d getGyroHeading() {
    return gyro.getRotation2d();
  }

  /**
   * Gets the positions of all swerve modules. Used for localization.
   *
   * @return An array of module positions.
   */
  public SwerveModulePosition[] getModulePositions() {
    return getSwerveData(SwerveModule::getPosition);
  }

  public SwerveModuleVelocity[] getModuleVelocities() {
    return getSwerveData(SwerveModule::getVelocity);
  }

  @Override
  public Command idle() {
    return runRepeatedly(
            () -> {
              frontLeft.stop();
              frontRight.stop();
              rearLeft.stop();
              rearRight.stop();
            })
        .named("Drive.Idle");
  }

  /**
   * A command that gives the driver full control of the drive.
   *
   * @param controller The driver controller.
   * @return a command that gives the driver full control of the drive
   */
  public Command driverControl(CommandGamepad controller) {
    return driveFieldRelative(
        () -> {
          double x = -controller.getLeftY();
          double y = -controller.getLeftX();
          double omega = -controller.getRightX();

          return new ChassisVelocities(
              DriveConstants.MAX_VELOCITY.times(x),
              DriveConstants.MAX_VELOCITY.times(y),
              DriveConstants.MAX_TURN_RATE.times(omega));
        });
  }

  /**
   * A command that aims the drivebase at a target. The command will exit when the necessary heading
   * is reached.
   *
   * @param robotPose A supplier for the current pose of the robot.
   * @param targetPose A supplier for the target pose to aim at.
   * @return The aiming command
   */
  public Command aimAt(Supplier<Pose2d> robotPose, Pose2d targetPose) {
    return driveFieldRelative(
            () -> {
              Pose2d robot = robotPose.get();
              Angle targetHeading = angleBetweenPoses(robot, targetPose);
              AngularVelocity omega =
                  RadiansPerSecond.of(
                      headingController.calculate(
                          robot.getRotation().getRadians(), targetHeading.in(Radians)));

              return new ChassisVelocities(MetersPerSecond.of(0), MetersPerSecond.of(0), omega);
            })
        .until(headingController::atSetpoint)
        .named("Drive.AimAt");
  }

  /**
   * A command that assists the driver in aiming the robot at a target pose. The driver still has
   * full control over the X and Y speeds, but heading is controlled automatically based on the
   * relative position of the robot and the target.
   *
   * @param controller The driver controller.
   * @param robotPose A supplier for the current robot pose.
   * @param targetPose A supplier for the target pose.
   * @return The aim-assist command.
   */
  public Command aimAssist(
      CommandGamepad controller, Supplier<Pose2d> robotPose, Supplier<Pose2d> targetPose) {
    return driveFieldRelative(
        () -> {
          // We use the driver's input to control X and Y speed as normal...
          double x = -controller.getLeftY();
          double y = -controller.getLeftX();

          // ... but heading is controlled automatically based on the relative position of the robot
          // and
          // the target.
          Pose2d robot = robotPose.get();
          Pose2d target = targetPose.get();
          Angle targetHeading = angleBetweenPoses(robot, target);
          AngularVelocity omega =
              RadiansPerSecond.of(
                  headingController.calculate(
                      robot.getRotation().getRadians(), targetHeading.in(Radians)));

          return new ChassisVelocities(
              DriveConstants.MAX_VELOCITY.times(x), DriveConstants.MAX_VELOCITY.times(y), omega);
        });
  }

  /**
   * Computes the angle from one pose to another. Useful for aiming the robot (positioned at the
   * source pose) at some target point on the field.
   *
   * @param source The source pose
   * @param target The target pose
   * @return The angle between the two poses
   */
  private static Angle angleBetweenPoses(Pose2d source, Pose2d target) {
    // atan2 conveniently returns a value in radians between -pi and pi, so we don't need to do
    // any additional calculations to normalize the angle
    return Radians.of(Math.atan2(target.getY() - source.getY(), target.getX() - source.getX()));
  }

  /**
   * Drives with a dynamic velocity. This command will never exit.
   *
   * @param velocities A supplier of chassis velocities. These velocities are expected to be in the
   *     field coordinate system.
   * @return a command that drives with the given velocities
   */
  public Command driveFieldRelative(Supplier<ChassisVelocities> velocities) {
    return runRepeatedly(
            () -> {
              ChassisVelocities velocity = velocities.get().toRobotRelative(getGyroHeading());
              SwerveModuleVelocity[] swerveModuleVelocities =
                  kinematics.toSwerveModuleVelocities(velocity);

              frontLeft.setTargetVelocity(swerveModuleVelocities[0]);
              frontRight.setTargetVelocity(swerveModuleVelocities[1]);
              rearLeft.setTargetVelocity(swerveModuleVelocities[2]);
              rearRight.setTargetVelocity(swerveModuleVelocities[3]);
            })
        .named("Drive.DriveSpeeds");
  }

  /**
   * Follows a path loaded from a file. The command will exit when the robot reaches the end of the
   * path.
   *
   * @param pathName the path to follow
   * @return a command that follows the path
   */
  public Command followPath(String pathName) {
    return run(coroutine -> {
          // Load the path
          PathFollower follower = PathFollower.load(pathName);

          // Run path follower in the background
          coroutine.fork(driveFieldRelative(follower::next));

          // Wait until we reach the end of the path, then exit
          coroutine.waitUntil(follower::isDone);
        })
        .named("Drive.FollowPath[" + pathName + "]");
  }

  @SuppressWarnings("unchecked")
  private <T> T[] getSwerveData(Function<SwerveModule, ? extends T> extractor) {
    T[] data = (T[]) new Object[4];
    data[0] = extractor.apply(frontLeft);
    data[1] = extractor.apply(frontRight);
    data[2] = extractor.apply(rearLeft);
    data[3] = extractor.apply(rearRight);
    return data;
  }
}
