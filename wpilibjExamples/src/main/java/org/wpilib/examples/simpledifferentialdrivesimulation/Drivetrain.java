// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.simpledifferentialdrivesimulation;

import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.DifferentialDriveOdometry;
import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.Models;
import org.wpilib.simulation.DifferentialDrivetrainSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.smartdashboard.Field2d;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

public class Drivetrain {
  // 3 meters per second.
  public static final double kMaxVelocity = 3.0;
  // 1/2 rotation per second.
  public static final double kMaxAngularVelocity = Math.PI;

  private static final double kTrackwidth = 0.381 * 2;
  private static final double kWheelRadius = 0.0508;
  private static final int kEncoderResolution = -4096;

  private final PWMSparkMax leftLeader = new PWMSparkMax(1);
  private final PWMSparkMax leftFollower = new PWMSparkMax(2);
  private final PWMSparkMax rightLeader = new PWMSparkMax(3);
  private final PWMSparkMax rightFollower = new PWMSparkMax(4);

  private final Encoder leftEncoder = new Encoder(0, 1);
  private final Encoder rightEncoder = new Encoder(2, 3);

  private final PIDController leftPIDController = new PIDController(8.5, 0, 0);
  private final PIDController rightPIDController = new PIDController(8.5, 0, 0);

  private final OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);

  private final DifferentialDriveKinematics kinematics =
      new DifferentialDriveKinematics(kTrackwidth);
  private final DifferentialDriveOdometry odometry =
      new DifferentialDriveOdometry(
          imu.getRotation2d(), leftEncoder.getDistance(), rightEncoder.getDistance());

  // Gains are for example purposes only - must be determined for your own
  // robot!
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(1, 3);

  // Simulation classes help us simulate our robot
  private final EncoderSim leftEncoderSim = new EncoderSim(leftEncoder);
  private final EncoderSim rightEncoderSim = new EncoderSim(rightEncoder);
  private final Field2d fieldSim = new Field2d();
  private final LinearSystem<N2, N2, N2> drivetrainSystem =
      Models.differentialDriveFromSysId(1.98, 0.2, 1.5, 0.3);
  private final DifferentialDrivetrainSim drivetrainSimulator =
      new DifferentialDrivetrainSim(
          drivetrainSystem, DCMotor.getCIM(2), 8, kTrackwidth, kWheelRadius, null);

  /** Subsystem constructor. */
  public Drivetrain() {
    leftLeader.addFollower(leftFollower);
    rightLeader.addFollower(rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.setInverted(true);

    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    leftEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);
    rightEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);

    leftEncoder.reset();
    rightEncoder.reset();

    rightLeader.setInverted(true);
    SmartDashboard.putData("Field", fieldSim);
  }

  /** Sets velocities to the drivetrain motors. */
  public void setVelocities(DifferentialDriveWheelVelocities velocities) {
    final double leftFeedforward = feedforward.calculate(velocities.left);
    final double rightFeedforward = feedforward.calculate(velocities.right);
    double leftOutput = leftPIDController.calculate(leftEncoder.getRate(), velocities.left);
    double rightOutput = rightPIDController.calculate(rightEncoder.getRate(), velocities.right);

    leftLeader.setVoltage(leftOutput + leftFeedforward);
    rightLeader.setVoltage(rightOutput + rightFeedforward);
  }

  /**
   * Controls the robot using arcade drive.
   *
   * @param xVelocity the velocity for the x axis
   * @param rot the rotation
   */
  public void drive(double xVelocity, double rot) {
    setVelocities(kinematics.toWheelVelocities(new ChassisVelocities(xVelocity, 0, rot)));
  }

  /** Update robot odometry. */
  public void updateOdometry() {
    odometry.update(imu.getRotation2d(), leftEncoder.getDistance(), rightEncoder.getDistance());
  }

  /** Resets robot odometry. */
  public void resetOdometry(Pose2d pose) {
    drivetrainSimulator.setPose(pose);
    odometry.resetPosition(
        imu.getRotation2d(), leftEncoder.getDistance(), rightEncoder.getDistance(), pose);
  }

  /** Check the current robot pose. */
  public Pose2d getPose() {
    return odometry.getPose();
  }

  /** Update our simulation. This should be run every robot loop in simulation. */
  public void simulationPeriodic() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated positions and velocities to our
    // simulated encoder and gyro. We negate the right side so that positive
    // voltages make the right side move forward.
    drivetrainSimulator.setInputs(
        leftLeader.getThrottle() * RobotController.getInputVoltage(),
        rightLeader.getThrottle() * RobotController.getInputVoltage());
    drivetrainSimulator.update(0.02);

    leftEncoderSim.setDistance(drivetrainSimulator.getLeftPosition());
    leftEncoderSim.setRate(drivetrainSimulator.getLeftVelocity());
    rightEncoderSim.setDistance(drivetrainSimulator.getRightPosition());
    rightEncoderSim.setRate(drivetrainSimulator.getRightVelocity());
    // gyroSim.setAngle(-drivetrainSimulator.getHeading().getDegrees());
  }

  /** Update odometry - this should be run every robot loop. */
  public void periodic() {
    updateOdometry();
    fieldSim.setRobotPose(odometry.getPose());
  }
}
