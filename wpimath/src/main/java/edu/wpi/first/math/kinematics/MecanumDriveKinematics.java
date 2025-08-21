// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.proto.MecanumDriveKinematicsProto;
import edu.wpi.first.math.kinematics.struct.MecanumDriveKinematicsStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import org.ejml.simple.SimpleMatrix;

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components) into individual
 * wheel speeds.
 *
 * <p>The inverse kinematics (converting from a desired chassis velocity to individual wheel speeds)
 * uses the relative locations of the wheels with respect to the center of rotation. The center of
 * rotation for inverse kinematics is also variable. This means that you can set your center of
 * rotation in a corner of the robot to perform special evasion maneuvers.
 *
 * <p>Forward kinematics (converting an array of wheel speeds into the overall chassis motion) is
 * performs the exact opposite of what inverse kinematics does. Since this is an overdetermined
 * system (more equations than variables), we use a least-squares approximation.
 *
 * <p>The inverse kinematics: [wheelSpeeds] = [wheelLocations] * [chassisSpeeds] We take the
 * Moore-Penrose pseudoinverse of [wheelLocations] and then multiply by [wheelSpeeds] to get our
 * chassis speeds.
 *
 * <p>Forward kinematics is also used for odometry -- determining the position of the robot on the
 * field using encoders and a gyro.
 */
public class MecanumDriveKinematics
    implements Kinematics<
            MecanumDriveWheelSpeeds, MecanumDriveWheelAccelerations, MecanumDriveWheelPositions>,
        ProtobufSerializable,
        StructSerializable {
  private final SimpleMatrix m_inverseKinematics;
  private final SimpleMatrix m_forwardKinematics;

  private final Translation2d m_frontLeftWheel;
  private final Translation2d m_frontRightWheel;
  private final Translation2d m_rearLeftWheel;
  private final Translation2d m_rearRightWheel;

  private Translation2d m_prevCoR = Translation2d.kZero;

  /** MecanumDriveKinematics protobuf for serialization. */
  public static final MecanumDriveKinematicsProto proto = new MecanumDriveKinematicsProto();

  /** MecanumDriveKinematics struct for serialization. */
  public static final MecanumDriveKinematicsStruct struct = new MecanumDriveKinematicsStruct();

  /**
   * Constructs a mecanum drive kinematics object.
   *
   * @param frontLeftWheel The location of the front-left wheel relative to the physical center of
   *     the robot, in meters.
   * @param frontRightWheel The location of the front-right wheel relative to the physical center of
   *     the robot, in meters.
   * @param rearLeftWheel The location of the rear-left wheel relative to the physical center of the
   *     robot, in meters.
   * @param rearRightWheel The location of the rear-right wheel relative to the physical center of
   *     the robot, in meters.
   */
  public MecanumDriveKinematics(
      Translation2d frontLeftWheel,
      Translation2d frontRightWheel,
      Translation2d rearLeftWheel,
      Translation2d rearRightWheel) {
    m_frontLeftWheel = frontLeftWheel;
    m_frontRightWheel = frontRightWheel;
    m_rearLeftWheel = rearLeftWheel;
    m_rearRightWheel = rearRightWheel;

    m_inverseKinematics = new SimpleMatrix(4, 3);

    setInverseKinematics(frontLeftWheel, frontRightWheel, rearLeftWheel, rearRightWheel);
    m_forwardKinematics = m_inverseKinematics.pseudoInverse();

    MathSharedStore.reportUsage("MecanumDriveKinematics", "");
  }

  /**
   * Performs inverse kinematics to return the wheel speeds from a desired chassis velocity. This
   * method is often used to convert joystick values into wheel speeds.
   *
   * <p>This function also supports variable centers of rotation. During normal operations, the
   * center of rotation is usually the same as the physical center of the robot; therefore, the
   * argument is defaulted to that use case. However, if you wish to change the center of rotation
   * for evasive maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @param centerOfRotation The center of rotation. For example, if you set the center of rotation
   *     at one corner of the robot and provide a chassis speed that only has a dtheta component,
   *     the robot will rotate around that corner.
   * @return The wheel speeds. Use caution because they are not normalized. Sometimes, a user input
   *     may cause one of the wheel speeds to go above the attainable max velocity. Use the {@link
   *     MecanumDriveWheelSpeeds#desaturate(double)} function to rectify this issue.
   */
  public MecanumDriveWheelSpeeds toWheelSpeeds(
      ChassisSpeeds chassisSpeeds, Translation2d centerOfRotation) {
    // We have a new center of rotation. We need to compute the matrix again.
    if (!centerOfRotation.equals(m_prevCoR)) {
      var fl = m_frontLeftWheel.minus(centerOfRotation);
      var fr = m_frontRightWheel.minus(centerOfRotation);
      var rl = m_rearLeftWheel.minus(centerOfRotation);
      var rr = m_rearRightWheel.minus(centerOfRotation);

      setInverseKinematics(fl, fr, rl, rr);
      m_prevCoR = centerOfRotation;
    }

    var chassisSpeedsVector = new SimpleMatrix(3, 1);
    chassisSpeedsVector.setColumn(0, 0, chassisSpeeds.vx, chassisSpeeds.vy, chassisSpeeds.omega);

    var wheelsVector = m_inverseKinematics.mult(chassisSpeedsVector);
    return new MecanumDriveWheelSpeeds(
        wheelsVector.get(0, 0),
        wheelsVector.get(1, 0),
        wheelsVector.get(2, 0),
        wheelsVector.get(3, 0));
  }

  /**
   * Performs inverse kinematics. See {@link #toWheelSpeeds(ChassisSpeeds, Translation2d)} for more
   * information.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @return The wheel speeds.
   */
  @Override
  public MecanumDriveWheelSpeeds toWheelSpeeds(ChassisSpeeds chassisSpeeds) {
    return toWheelSpeeds(chassisSpeeds, Translation2d.kZero);
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the given wheel speeds.
   * This method is often used for odometry -- determining the robot's position on the field using
   * data from the real-world speed of each wheel on the robot.
   *
   * @param wheelSpeeds The current mecanum drive wheel speeds.
   * @return The resulting chassis speed.
   */
  @Override
  public ChassisSpeeds toChassisSpeeds(MecanumDriveWheelSpeeds wheelSpeeds) {
    var wheelSpeedsVector = new SimpleMatrix(4, 1);
    wheelSpeedsVector.setColumn(
        0,
        0,
        wheelSpeeds.frontLeft,
        wheelSpeeds.frontRight,
        wheelSpeeds.rearLeft,
        wheelSpeeds.rearRight);
    var chassisSpeedsVector = m_forwardKinematics.mult(wheelSpeedsVector);

    return new ChassisSpeeds(
        chassisSpeedsVector.get(0, 0),
        chassisSpeedsVector.get(1, 0),
        chassisSpeedsVector.get(2, 0));
  }

  /**
   * Performs inverse kinematics to return the wheel accelerations from a desired chassis
   * acceleration. This method is often used for dynamics calculations -- converting desired robot
   * accelerations into individual wheel accelerations.
   *
   * <p>This function also supports variable centers of rotation. During normal operations, the
   * center of rotation is usually the same as the physical center of the robot; therefore, the
   * argument is defaulted to that use case. However, if you wish to change the center of rotation
   * for evasive maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @param centerOfRotation The center of rotation. For example, if you set the center of rotation
   *     at one corner of the robot and provide a chassis acceleration that only has a dtheta
   *     component, the robot will rotate around that corner.
   * @return The wheel accelerations.
   */
  public MecanumDriveWheelAccelerations toWheelAccelerations(
      ChassisAccelerations chassisAccelerations, Translation2d centerOfRotation) {
    // We have a new center of rotation. We need to compute the matrix again.
    if (!centerOfRotation.equals(m_prevCoR)) {
      var fl = m_frontLeftWheel.minus(centerOfRotation);
      var fr = m_frontRightWheel.minus(centerOfRotation);
      var rl = m_rearLeftWheel.minus(centerOfRotation);
      var rr = m_rearRightWheel.minus(centerOfRotation);

      setInverseKinematics(fl, fr, rl, rr);
      m_prevCoR = centerOfRotation;
    }

    var chassisAccelerationsVector = new SimpleMatrix(3, 1);
    chassisAccelerationsVector.setColumn(
        0, 0, chassisAccelerations.ax, chassisAccelerations.ay, chassisAccelerations.alpha);

    var wheelsVector = m_inverseKinematics.mult(chassisAccelerationsVector);
    return new MecanumDriveWheelAccelerations(
        wheelsVector.get(0, 0),
        wheelsVector.get(1, 0),
        wheelsVector.get(2, 0),
        wheelsVector.get(3, 0));
  }

  /**
   * Performs inverse kinematics. See {@link #toWheelAccelerations(ChassisAccelerations,
   * Translation2d)} for more information.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @return The wheel accelerations.
   */
  @Override
  public MecanumDriveWheelAccelerations toWheelAccelerations(
      ChassisAccelerations chassisAccelerations) {
    return toWheelAccelerations(chassisAccelerations, Translation2d.kZero);
  }

  /**
   * Performs forward kinematics to return the resulting chassis accelerations from the given wheel
   * accelerations. This method is often used for dynamics calculations -- determining the robot's
   * acceleration on the field using data from the real-world acceleration of each wheel on the
   * robot.
   *
   * @param wheelAccelerations The current mecanum drive wheel accelerations.
   * @return The resulting chassis accelerations.
   */
  @Override
  public ChassisAccelerations toChassisAccelerations(
      MecanumDriveWheelAccelerations wheelAccelerations) {
    var wheelAccelerationsVector = new SimpleMatrix(4, 1);
    wheelAccelerationsVector.setColumn(
        0,
        0,
        wheelAccelerations.frontLeft,
        wheelAccelerations.frontRight,
        wheelAccelerations.rearLeft,
        wheelAccelerations.rearRight);
    var chassisAccelerationsVector = m_forwardKinematics.mult(wheelAccelerationsVector);

    return new ChassisAccelerations(
        chassisAccelerationsVector.get(0, 0),
        chassisAccelerationsVector.get(1, 0),
        chassisAccelerationsVector.get(2, 0));
  }

  @Override
  public Twist2d toTwist2d(MecanumDriveWheelPositions start, MecanumDriveWheelPositions end) {
    var wheelDeltasVector = new SimpleMatrix(4, 1);
    wheelDeltasVector.setColumn(
        0,
        0,
        end.frontLeft - start.frontLeft,
        end.frontRight - start.frontRight,
        end.rearLeft - start.rearLeft,
        end.rearRight - start.rearRight);
    var twist = m_forwardKinematics.mult(wheelDeltasVector);
    return new Twist2d(twist.get(0, 0), twist.get(1, 0), twist.get(2, 0));
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given wheel deltas. This
   * method is often used for odometry -- determining the robot's position on the field using
   * changes in the distance driven by each wheel on the robot.
   *
   * @param wheelDeltas The distances driven by each wheel.
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(MecanumDriveWheelPositions wheelDeltas) {
    var wheelDeltasVector = new SimpleMatrix(4, 1);
    wheelDeltasVector.setColumn(
        0,
        0,
        wheelDeltas.frontLeft,
        wheelDeltas.frontRight,
        wheelDeltas.rearLeft,
        wheelDeltas.rearRight);
    var twist = m_forwardKinematics.mult(wheelDeltasVector);
    return new Twist2d(twist.get(0, 0), twist.get(1, 0), twist.get(2, 0));
  }

  /**
   * Construct inverse kinematics matrix from wheel locations.
   *
   * @param fl The location of the front-left wheel relative to the physical center of the robot.
   * @param fr The location of the front-right wheel relative to the physical center of the robot.
   * @param rl The location of the rear-left wheel relative to the physical center of the robot.
   * @param rr The location of the rear-right wheel relative to the physical center of the robot.
   */
  private void setInverseKinematics(
      Translation2d fl, Translation2d fr, Translation2d rl, Translation2d rr) {
    m_inverseKinematics.setRow(0, 0, 1, -1, -(fl.getX() + fl.getY()));
    m_inverseKinematics.setRow(1, 0, 1, 1, fr.getX() - fr.getY());
    m_inverseKinematics.setRow(2, 0, 1, 1, rl.getX() - rl.getY());
    m_inverseKinematics.setRow(3, 0, 1, -1, -(rr.getX() + rr.getY()));
  }

  /**
   * Returns the front-left wheel translation.
   *
   * @return The front-left wheel translation.
   */
  public Translation2d getFrontLeft() {
    return m_frontLeftWheel;
  }

  /**
   * Returns the front-right wheel translation.
   *
   * @return The front-right wheel translation.
   */
  public Translation2d getFrontRight() {
    return m_frontRightWheel;
  }

  /**
   * Returns the rear-left wheel translation.
   *
   * @return The rear-left wheel translation.
   */
  public Translation2d getRearLeft() {
    return m_rearLeftWheel;
  }

  /**
   * Returns the rear-right wheel translation.
   *
   * @return The rear-right wheel translation.
   */
  public Translation2d getRearRight() {
    return m_rearRightWheel;
  }

  @Override
  public MecanumDriveWheelPositions copy(MecanumDriveWheelPositions positions) {
    return new MecanumDriveWheelPositions(
        positions.frontLeft, positions.frontRight, positions.rearLeft, positions.rearRight);
  }

  @Override
  public void copyInto(MecanumDriveWheelPositions positions, MecanumDriveWheelPositions output) {
    output.frontLeft = positions.frontLeft;
    output.frontRight = positions.frontRight;
    output.rearLeft = positions.rearLeft;
    output.rearRight = positions.rearRight;
  }

  @Override
  public MecanumDriveWheelPositions interpolate(
      MecanumDriveWheelPositions startValue, MecanumDriveWheelPositions endValue, double t) {
    return startValue.interpolate(endValue, t);
  }
}
