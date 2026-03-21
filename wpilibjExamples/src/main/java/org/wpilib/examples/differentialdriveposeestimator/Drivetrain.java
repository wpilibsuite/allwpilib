// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.differentialdriveposeestimator;

import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.estimator.DifferentialDrivePoseEstimator;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Quaternion;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Transform3d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.Models;
import org.wpilib.math.util.ComputerVisionUtil;
import org.wpilib.math.util.Units;
import org.wpilib.networktables.DoubleArrayEntry;
import org.wpilib.networktables.DoubleArrayTopic;
import org.wpilib.simulation.DifferentialDrivetrainSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.smartdashboard.Field2d;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;
import org.wpilib.system.Timer;
import org.wpilib.vision.apriltag.AprilTagFieldLayout;
import org.wpilib.vision.apriltag.AprilTagFields;

/** Represents a differential drive style drivetrain. */
public class Drivetrain {
  public static final double kMaxVelocity = 3.0; // meters per second
  public static final double kMaxAngularVelocity = 2 * Math.PI; // one rotation per second

  private static final double kTrackwidth = 0.381 * 2; // meters
  private static final double kWheelRadius = 0.0508; // meters
  private static final int kEncoderResolution = 4096;

  private final PWMSparkMax leftLeader = new PWMSparkMax(1);
  private final PWMSparkMax leftFollower = new PWMSparkMax(2);
  private final PWMSparkMax rightLeader = new PWMSparkMax(3);
  private final PWMSparkMax rightFollower = new PWMSparkMax(4);

  private final Encoder leftEncoder = new Encoder(0, 1);
  private final Encoder rightEncoder = new Encoder(2, 3);

  private final OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);

  private final PIDController leftPIDController = new PIDController(1, 0, 0);
  private final PIDController rightPIDController = new PIDController(1, 0, 0);

  private final DifferentialDriveKinematics kinematics =
      new DifferentialDriveKinematics(kTrackwidth);

  private final Pose3d objectInField;

  private final Transform3d robotToCamera =
      new Transform3d(new Translation3d(1, 1, 1), new Rotation3d(0, 0, Math.PI / 2));

  private final DoubleArrayEntry cameraToObjectEntry;

  private final double[] defaultVal = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  private final Field2d fieldSim = new Field2d();
  private final Field2d fieldApproximation = new Field2d();

  /* Here we use DifferentialDrivePoseEstimator so that we can fuse odometry readings. The
  numbers used  below are robot specific, and should be tuned. */
  private final DifferentialDrivePoseEstimator poseEstimator =
      new DifferentialDrivePoseEstimator(
          kinematics,
          imu.getRotation2d(),
          leftEncoder.getDistance(),
          rightEncoder.getDistance(),
          Pose2d.kZero,
          VecBuilder.fill(0.05, 0.05, Units.degreesToRadians(5)),
          VecBuilder.fill(0.5, 0.5, Units.degreesToRadians(30)));

  // Gains are for example purposes only - must be determined for your own robot!
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(1, 3);

  // Simulation classes
  private final EncoderSim leftEncoderSim = new EncoderSim(leftEncoder);
  private final EncoderSim rightEncoderSim = new EncoderSim(rightEncoder);
  private final LinearSystem<N2, N2, N2> drivetrainSystem =
      Models.differentialDriveFromSysId(1.98, 0.2, 1.5, 0.3);
  private final DifferentialDrivetrainSim drivetrainSimulator =
      new DifferentialDrivetrainSim(
          drivetrainSystem, DCMotor.getCIM(2), 8, kTrackwidth, kWheelRadius, null);

  /**
   * Constructs a differential drive object. Sets the encoder distance per pulse and resets the
   * gyro.
   */
  public Drivetrain(DoubleArrayTopic cameraToObjectTopic) {
    imu.resetYaw();

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

    cameraToObjectEntry = cameraToObjectTopic.getEntry(defaultVal);

    objectInField =
        AprilTagFieldLayout.loadField(AprilTagFields.k2024Crescendo).getTagPose(0).get();

    SmartDashboard.putData("Field", fieldSim);
    SmartDashboard.putData("FieldEstimation", fieldApproximation);
  }

  /**
   * Sets the desired wheel velocities.
   *
   * @param velocities The desired wheel velocities.
   */
  public void setVelocities(DifferentialDriveWheelVelocities velocities) {
    final double leftFeedforward = feedforward.calculate(velocities.left);
    final double rightFeedforward = feedforward.calculate(velocities.right);

    final double leftOutput = leftPIDController.calculate(leftEncoder.getRate(), velocities.left);
    final double rightOutput =
        rightPIDController.calculate(rightEncoder.getRate(), velocities.right);
    leftLeader.setVoltage(leftOutput + leftFeedforward);
    rightLeader.setVoltage(rightOutput + rightFeedforward);
  }

  /**
   * Drives the robot with the given linear velocity and angular velocity.
   *
   * @param xVelocity Linear velocity in m/s.
   * @param rot Angular velocity in rad/s.
   */
  public void drive(double xVelocity, double rot) {
    var wheelVelocities = kinematics.toWheelVelocities(new ChassisVelocities(xVelocity, 0.0, rot));
    setVelocities(wheelVelocities);
  }

  /**
   * Computes and publishes to a network tables topic the transformation from the camera's pose to
   * the object's pose. This function exists solely for the purposes of simulation, and this would
   * normally be handled by computer vision.
   *
   * <p>The object could be a target or a fiducial marker.
   *
   * @param objectInField The object's field-relative position.
   * @param robotToCamera The transformation from the robot's pose to the camera's pose.
   * @param cameraToObjectEntry The networktables entry publishing and querying example computer
   *     vision measurements.
   */
  public void publishCameraToObject(
      Pose3d objectInField,
      Transform3d robotToCamera,
      DoubleArrayEntry cameraToObjectEntry,
      DifferentialDrivetrainSim drivetrainSimulator) {
    Pose3d robotInField = new Pose3d(drivetrainSimulator.getPose());
    Pose3d cameraInField = robotInField.plus(robotToCamera);
    Transform3d cameraToObject = new Transform3d(cameraInField, objectInField);

    // Publishes double array with Translation3D elements {x, y, z} and Rotation3D elements {w, x,
    // y, z} which describe
    // the cameraToObject transformation.
    double[] val = {
      cameraToObject.getX(),
      cameraToObject.getY(),
      cameraToObject.getZ(),
      cameraToObject.getRotation().getQuaternion().getW(),
      cameraToObject.getRotation().getQuaternion().getX(),
      cameraToObject.getRotation().getQuaternion().getY(),
      cameraToObject.getRotation().getQuaternion().getZ()
    };
    cameraToObjectEntry.set(val);
  }

  /**
   * Queries the camera-to-object transformation from networktables to compute the robot's
   * field-relative pose from vision measurements.
   *
   * <p>The object could be a target or a fiducial marker.
   *
   * @param objectInField The object's field-relative pose.
   * @param robotToCamera The transformation from the robot's pose to the camera's pose.
   * @param cameraToObjectEntry The networktables entry publishing and querying example computer
   *     vision measurements.
   */
  public Pose3d objectToRobotPose(
      Pose3d objectInField, Transform3d robotToCamera, DoubleArrayEntry cameraToObjectEntry) {
    double[] val = cameraToObjectEntry.get();

    // Reconstruct cameraToObject Transform3D from networktables.
    Translation3d translation = new Translation3d(val[0], val[1], val[2]);
    Rotation3d rotation = new Rotation3d(new Quaternion(val[3], val[4], val[5], val[6]));
    Transform3d cameraToObject = new Transform3d(translation, rotation);

    return ComputerVisionUtil.objectToRobotPose(objectInField, cameraToObject, robotToCamera);
  }

  /** Updates the field-relative position. */
  public void updateOdometry() {
    poseEstimator.update(
        imu.getRotation2d(), leftEncoder.getDistance(), rightEncoder.getDistance());

    // Publish cameraToObject transformation to networktables --this would normally be handled by
    // the
    // computer vision solution.
    publishCameraToObject(objectInField, robotToCamera, cameraToObjectEntry, drivetrainSimulator);

    // Compute the robot's field-relative position exclusively from vision measurements.
    Pose3d visionMeasurement3d =
        objectToRobotPose(objectInField, robotToCamera, cameraToObjectEntry);

    // Convert robot pose from Pose3d to Pose2d needed to apply vision measurements.
    Pose2d visionMeasurement2d = visionMeasurement3d.toPose2d();

    // Apply vision measurements. For simulation purposes only, we don't input a latency delay -- on
    // a real robot, this must be calculated based either on known latency or timestamps.
    poseEstimator.addVisionMeasurement(visionMeasurement2d, Timer.getTimestamp());
  }

  /** This function is called periodically during simulation. */
  public void simulationPeriodic() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated positions and velocities to our
    // simulated encoder and gyro.
    drivetrainSimulator.setInputs(
        leftLeader.getThrottle() * RobotController.getInputVoltage(),
        rightLeader.getThrottle() * RobotController.getInputVoltage());
    drivetrainSimulator.update(0.02);

    leftEncoderSim.setDistance(drivetrainSimulator.getLeftPosition());
    leftEncoderSim.setRate(drivetrainSimulator.getLeftVelocity());
    rightEncoderSim.setDistance(drivetrainSimulator.getRightPosition());
    rightEncoderSim.setRate(drivetrainSimulator.getRightVelocity());
    // gyroSim.setAngle(-drivetrainSimulator.getHeading().getDegrees()); // TODO(Ryan): fixup
    // when sim implemented
  }

  /** This function is called periodically, no matter the mode. */
  public void periodic() {
    updateOdometry();
    fieldSim.setRobotPose(drivetrainSimulator.getPose());
    fieldApproximation.setRobotPose(poseEstimator.getEstimatedPosition());
  }
}
