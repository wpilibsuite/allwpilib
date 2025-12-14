// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.RadiansPerSecond;

import java.util.Arrays;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Twist2d;
import org.wpilib.math.kinematics.proto.SwerveDriveKinematicsProto;
import org.wpilib.math.kinematics.struct.SwerveDriveKinematicsStruct;
import org.wpilib.math.util.MathSharedStore;
import org.wpilib.units.measure.AngularVelocity;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components) into individual
 * module states (velocity and angle).
 *
 * <p>The inverse kinematics (converting from a desired chassis velocity to individual module
 * states) uses the relative locations of the modules with respect to the center of rotation. The
 * center of rotation for inverse kinematics is also variable. This means that you can set your
 * center of rotation in a corner of the robot to perform special evasion maneuvers.
 *
 * <p>Forward kinematics (converting an array of module states into the overall chassis motion) is
 * performs the exact opposite of what inverse kinematics does. Since this is an overdetermined
 * system (more equations than variables), we use a least-squares approximation.
 *
 * <p>The inverse kinematics: [moduleVelocities] = [moduleLocations] * [chassisVelocities] We take
 * the Moore-Penrose pseudoinverse of [moduleLocations] and then multiply by [moduleVelocities] to
 * get our chassis velocities.
 *
 * <p>Forward kinematics is also used for odometry -- determining the position of the robot on the
 * field using encoders and a gyro.
 */
@SuppressWarnings("overrides")
public class SwerveDriveKinematics
    implements Kinematics<
            SwerveModulePosition[], SwerveModuleVelocity[], SwerveModuleAcceleration[]>,
        ProtobufSerializable,
        StructSerializable {
  private final SimpleMatrix m_firstOrderInverseKinematics;
  private final SimpleMatrix m_firstOrderForwardKinematics;
  private final SimpleMatrix m_secondOrderInverseKinematics;
  private final SimpleMatrix m_secondOrderForwardKinematics;

  private final int m_numModules;
  private final Translation2d[] m_modules;
  private Rotation2d[] m_moduleHeadings;
  private Translation2d m_prevCoR = Translation2d.kZero;

  /**
   * Constructs a swerve drive kinematics object. This takes in a variable number of module
   * locations as Translation2d objects. The order in which you pass in the module locations is the
   * same order that you will receive the module states when performing inverse kinematics. It is
   * also expected that you pass in the module states in the same order when calling the forward
   * kinematics methods.
   *
   * @param moduleTranslations The locations of the modules relative to the physical center of the
   *     robot.
   */
  public SwerveDriveKinematics(Translation2d... moduleTranslations) {
    if (moduleTranslations.length < 2) {
      throw new IllegalArgumentException("A swerve drive requires at least two modules");
    }
    m_numModules = moduleTranslations.length;
    m_modules = Arrays.copyOf(moduleTranslations, m_numModules);
    m_moduleHeadings = new Rotation2d[m_numModules];
    Arrays.fill(m_moduleHeadings, Rotation2d.kZero);
    m_firstOrderInverseKinematics = new SimpleMatrix(m_numModules * 2, 3);
    m_secondOrderInverseKinematics = new SimpleMatrix(m_numModules * 2, 4);

    setInverseKinematics(Translation2d.kZero);

    m_firstOrderForwardKinematics = m_firstOrderInverseKinematics.pseudoInverse();
    m_secondOrderForwardKinematics = m_secondOrderInverseKinematics.pseudoInverse();

    MathSharedStore.reportUsage("SwerveDriveKinematics", "");
  }

  /**
   * Reset the internal swerve module headings.
   *
   * @param moduleHeadings The swerve module headings. The order of the module headings should be
   *     same as passed into the constructor of this class.
   */
  public void resetHeadings(Rotation2d... moduleHeadings) {
    if (moduleHeadings.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of headings is not consistent with number of module locations provided in "
              + "constructor");
    }
    m_moduleHeadings = Arrays.copyOf(moduleHeadings, m_numModules);
  }

  /**
   * Performs inverse kinematics to return the module states from a desired chassis velocity. This
   * method is often used to convert joystick values into module velocities and angles.
   *
   * <p>This function also supports variable centers of rotation. During normal operations, the
   * center of rotation is usually the same as the physical center of the robot; therefore, the
   * argument is defaulted to that use case. However, if you wish to change the center of rotation
   * for evasive maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * <p>In the case that the desired chassis velocities are zero (i.e. the robot will be
   * stationary), the previously calculated module angle will be maintained.
   *
   * @param chassisVelocities The desired chassis velocity.
   * @param centerOfRotation The center of rotation. For example, if you set the center of rotation
   *     at one corner of the robot and provide a chassis velocity that only has a dtheta component,
   *     the robot will rotate around that corner.
   * @return An array containing the module states. Use caution because these module states are not
   *     normalized. Sometimes, a user input may cause one of the module velocities to go above the
   *     attainable max velocity. Use the {@link #desaturateWheelVelocities(SwerveModuleVelocity[],
   *     double) DesaturateWheelVelocities} function to rectify this issue.
   */
  public SwerveModuleVelocity[] toSwerveModuleVelocities(
      ChassisVelocities chassisVelocities, Translation2d centerOfRotation) {
    var moduleVelocities = new SwerveModuleVelocity[m_numModules];

    if (chassisVelocities.vx == 0.0
        && chassisVelocities.vy == 0.0
        && chassisVelocities.omega == 0.0) {
      for (int i = 0; i < m_numModules; i++) {
        moduleVelocities[i] = new SwerveModuleVelocity(0.0, m_moduleHeadings[i]);
      }

      return moduleVelocities;
    }

    if (!centerOfRotation.equals(m_prevCoR)) {
      setInverseKinematics(centerOfRotation);
    }

    var chassisVelocitiesVector = new SimpleMatrix(3, 1);
    chassisVelocitiesVector.setColumn(
        0, 0, chassisVelocities.vx, chassisVelocities.vy, chassisVelocities.omega);

    var moduleVelocitiesMatrix = m_firstOrderInverseKinematics.mult(chassisVelocitiesVector);

    for (int i = 0; i < m_numModules; i++) {
      double x = moduleVelocitiesMatrix.get(i * 2, 0);
      double y = moduleVelocitiesMatrix.get(i * 2 + 1, 0);

      double velocity = Math.hypot(x, y);
      Rotation2d angle = velocity > 1e-6 ? new Rotation2d(x, y) : m_moduleHeadings[i];

      moduleVelocities[i] = new SwerveModuleVelocity(velocity, angle);
      m_moduleHeadings[i] = angle;
    }

    return moduleVelocities;
  }

  /**
   * Performs inverse kinematics. See {@link #toSwerveModuleVelocities(ChassisVelocities,
   * Translation2d)} toSwerveModuleVelocities for more information.
   *
   * @param chassisVelocities The desired chassis velocity.
   * @return An array containing the module states.
   */
  public SwerveModuleVelocity[] toSwerveModuleVelocities(ChassisVelocities chassisVelocities) {
    return toSwerveModuleVelocities(chassisVelocities, Translation2d.kZero);
  }

  @Override
  public SwerveModuleVelocity[] toWheelVelocities(ChassisVelocities chassisVelocities) {
    return toSwerveModuleVelocities(chassisVelocities);
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the given module states.
   * This method is often used for odometry -- determining the robot's position on the field using
   * data from the real-world velocity and angle of each module on the robot.
   *
   * @param moduleVelocities The state of the modules (as a SwerveModuleVelocity type) as measured
   *     from respective encoders and gyros. The order of the swerve module states should be same as
   *     passed into the constructor of this class.
   * @return The resulting chassis velocity.
   */
  @Override
  public ChassisVelocities toChassisVelocities(SwerveModuleVelocity... moduleVelocities) {
    if (moduleVelocities.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of module locations provided in "
              + "constructor");
    }
    var moduleVelocitiesMatrix = new SimpleMatrix(m_numModules * 2, 1);

    for (int i = 0; i < m_numModules; i++) {
      var module = moduleVelocities[i];
      moduleVelocitiesMatrix.set(i * 2, 0, module.velocity * module.angle.getCos());
      moduleVelocitiesMatrix.set(i * 2 + 1, module.velocity * module.angle.getSin());
    }

    var chassisVelocitiesVector = m_firstOrderForwardKinematics.mult(moduleVelocitiesMatrix);
    return new ChassisVelocities(
        chassisVelocitiesVector.get(0, 0),
        chassisVelocitiesVector.get(1, 0),
        chassisVelocitiesVector.get(2, 0));
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the given module states.
   * This method is often used for odometry -- determining the robot's position on the field using
   * data from the real-world velocity and angle of each module on the robot.
   *
   * @param moduleDeltas The latest change in position of the modules (as a SwerveModulePosition
   *     type) as measured from respective encoders and gyros. The order of the swerve module states
   *     should be same as passed into the constructor of this class.
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(SwerveModulePosition... moduleDeltas) {
    if (moduleDeltas.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of module locations provided in "
              + "constructor");
    }
    var moduleDeltaMatrix = new SimpleMatrix(m_numModules * 2, 1);

    for (int i = 0; i < m_numModules; i++) {
      var module = moduleDeltas[i];
      moduleDeltaMatrix.set(i * 2, 0, module.distance * module.angle.getCos());
      moduleDeltaMatrix.set(i * 2 + 1, 0, module.distance * module.angle.getSin());
    }

    var chassisDeltaVector = m_firstOrderForwardKinematics.mult(moduleDeltaMatrix);
    return new Twist2d(
        chassisDeltaVector.get(0, 0), chassisDeltaVector.get(1, 0), chassisDeltaVector.get(2, 0));
  }

  @Override
  public Twist2d toTwist2d(SwerveModulePosition[] start, SwerveModulePosition[] end) {
    if (start.length != end.length) {
      throw new IllegalArgumentException("Inconsistent number of modules!");
    }
    var newPositions = new SwerveModulePosition[start.length];
    for (int i = 0; i < start.length; i++) {
      newPositions[i] = new SwerveModulePosition(end[i].distance - start[i].distance, end[i].angle);
    }
    return toTwist2d(newPositions);
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more modules may be
   * above the max attainable velocity for the driving motor on that module. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between modules.
   *
   * <p>Scaling down the module velocities rotates the direction of net motion in the opposite
   * direction of rotational velocity, which makes discretizing the chassis velocities inaccurate
   * because the discretization did not account for this translational skew.
   *
   * @param moduleVelocities Reference to array of module states. The array will be mutated with the
   *     normalized velocities!
   * @param attainableMaxVelocity The absolute max velocity in meters per second that a module can
   *     reach.
   */
  public static void desaturateWheelVelocities(
      SwerveModuleVelocity[] moduleVelocities, double attainableMaxVelocity) {
    double realMaxVelocity = 0;
    for (SwerveModuleVelocity moduleVelocity : moduleVelocities) {
      realMaxVelocity = Math.max(realMaxVelocity, Math.abs(moduleVelocity.velocity));
    }
    if (realMaxVelocity > attainableMaxVelocity) {
      for (SwerveModuleVelocity moduleVelocity : moduleVelocities) {
        moduleVelocity.velocity = moduleVelocity.velocity / realMaxVelocity * attainableMaxVelocity;
      }
    }
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more modules may be
   * above the max attainable velocity for the driving motor on that module. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between modules.
   *
   * <p>Scaling down the module velocities rotates the direction of net motion in the opposite
   * direction of rotational velocity, which makes discretizing the chassis velocities inaccurate
   * because the discretization did not account for this translational skew.
   *
   * @param moduleVelocities Reference to array of module states. The array will be mutated with the
   *     normalized velocities!
   * @param attainableMaxVelocity The absolute max velocity in meters per second that a module can
   *     reach.
   */
  public static void desaturateWheelVelocities(
      SwerveModuleVelocity[] moduleVelocities, LinearVelocity attainableMaxVelocity) {
    desaturateWheelVelocities(moduleVelocities, attainableMaxVelocity.in(MetersPerSecond));
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the specified maximum, as
   * well as getting rid of joystick saturation at edges of joystick.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more modules may be
   * above the max attainable velocity for the driving motor on that module. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between modules.
   *
   * <p>Scaling down the module velocities rotates the direction of net motion in the opposite
   * direction of rotational velocity, which makes discretizing the chassis velocities inaccurate
   * because the discretization did not account for this translational skew.
   *
   * @param moduleVelocities Reference to array of module states. The array will be mutated with the
   *     normalized velocities!
   * @param desiredChassisVelocity The desired velocity of the robot
   * @param attainableMaxModuleVelocity The absolute max velocity in meters per second that a module
   *     can reach
   * @param attainableMaxTranslationalVelocity The absolute max velocity in meters per second that
   *     your robot can reach while translating
   * @param attainableMaxRotationalVelocity The absolute max velocity in radians per second the
   *     robot can reach while rotating
   */
  public static void desaturateWheelVelocities(
      SwerveModuleVelocity[] moduleVelocities,
      ChassisVelocities desiredChassisVelocity,
      double attainableMaxModuleVelocity,
      double attainableMaxTranslationalVelocity,
      double attainableMaxRotationalVelocity) {
    double realMaxVelocity = 0;
    for (SwerveModuleVelocity moduleVelocity : moduleVelocities) {
      realMaxVelocity = Math.max(realMaxVelocity, Math.abs(moduleVelocity.velocity));
    }

    if (attainableMaxTranslationalVelocity == 0
        || attainableMaxRotationalVelocity == 0
        || realMaxVelocity == 0) {
      return;
    }
    double translationalK =
        Math.hypot(desiredChassisVelocity.vx, desiredChassisVelocity.vy)
            / attainableMaxTranslationalVelocity;
    double rotationalK = Math.abs(desiredChassisVelocity.omega) / attainableMaxRotationalVelocity;
    double k = Math.max(translationalK, rotationalK);
    double scale = Math.min(k * attainableMaxModuleVelocity / realMaxVelocity, 1);
    for (SwerveModuleVelocity moduleVelocity : moduleVelocities) {
      moduleVelocity.velocity *= scale;
    }
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the specified maximum, as
   * well as getting rid of joystick saturation at edges of joystick.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more modules may be
   * above the max attainable velocity for the driving motor on that module. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between modules.
   *
   * <p>Scaling down the module velocities rotates the direction of net motion in the opposite
   * direction of rotational velocity, which makes discretizing the chassis velocities inaccurate
   * because the discretization did not account for this translational skew.
   *
   * @param moduleVelocities Reference to array of module states. The array will be mutated with the
   *     normalized velocities!
   * @param desiredChassisVelocity The desired velocity of the robot
   * @param attainableMaxModuleVelocity The absolute max velocity that a module can reach
   * @param attainableMaxTranslationalVelocity The absolute max velocity that your robot can reach
   *     while translating
   * @param attainableMaxRotationalVelocity The absolute max velocity the robot can reach while
   *     rotating
   */
  public static void desaturateWheelVelocities(
      SwerveModuleVelocity[] moduleVelocities,
      ChassisVelocities desiredChassisVelocity,
      LinearVelocity attainableMaxModuleVelocity,
      LinearVelocity attainableMaxTranslationalVelocity,
      AngularVelocity attainableMaxRotationalVelocity) {
    desaturateWheelVelocities(
        moduleVelocities,
        desiredChassisVelocity,
        attainableMaxModuleVelocity.in(MetersPerSecond),
        attainableMaxTranslationalVelocity.in(MetersPerSecond),
        attainableMaxRotationalVelocity.in(RadiansPerSecond));
  }

  @Override
  public SwerveModulePosition[] copy(SwerveModulePosition[] positions) {
    var newPositions = new SwerveModulePosition[positions.length];
    for (int i = 0; i < positions.length; ++i) {
      newPositions[i] = positions[i].copy();
    }
    return newPositions;
  }

  @Override
  public void copyInto(SwerveModulePosition[] positions, SwerveModulePosition[] output) {
    if (positions.length != output.length) {
      throw new IllegalArgumentException("Inconsistent number of modules!");
    }
    for (int i = 0; i < positions.length; ++i) {
      output[i].distance = positions[i].distance;
      output[i].angle = positions[i].angle;
    }
  }

  @Override
  public SwerveModulePosition[] interpolate(
      SwerveModulePosition[] startValue, SwerveModulePosition[] endValue, double t) {
    if (endValue.length != startValue.length) {
      throw new IllegalArgumentException("Inconsistent number of modules!");
    }
    var newPositions = new SwerveModulePosition[startValue.length];
    for (int i = 0; i < startValue.length; ++i) {
      newPositions[i] = startValue[i].interpolate(endValue[i], t);
    }
    return newPositions;
  }

  /**
   * Gets the locations of the modules relative to the center of rotation.
   *
   * @return The locations of the modules relative to the center of rotation. This array should not
   *     be modified.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public Translation2d[] getModules() {
    return m_modules;
  }

  /** SwerveDriveKinematics protobuf for serialization. */
  public static final SwerveDriveKinematicsProto proto = new SwerveDriveKinematicsProto();

  /**
   * Creates an implementation of the {@link Struct} interface for swerve drive kinematics objects.
   *
   * @param numModules The number of modules of the kinematics objects this serializer processes.
   * @return The struct implementation.
   */
  public static final SwerveDriveKinematicsStruct getStruct(int numModules) {
    return new SwerveDriveKinematicsStruct(numModules);
  }

  /**
   * Performs inverse kinematics to return the module accelerations from a desired chassis
   * acceleration. This method is often used for dynamics calculations -- converting desired robot
   * accelerations into individual module accelerations.
   *
   * <p>This function also supports variable centers of rotation. During normal operations, the
   * center of rotation is usually the same as the physical center of the robot; therefore, the
   * argument is defaulted to that use case. However, if you wish to change the center of rotation
   * for evasive maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @param angularVelocity The desired robot angular velocity.
   * @param centerOfRotation The center of rotation. For example, if you set the center of rotation
   *     at one corner of the robot and provide a chassis acceleration that only has a dtheta
   *     component, the robot will rotate around that corner.
   * @return An array containing the module accelerations.
   */
  public SwerveModuleAcceleration[] toSwerveModuleAccelerations(
      ChassisAccelerations chassisAccelerations,
      double angularVelocity,
      Translation2d centerOfRotation) {
    // Derivation for second-order kinematics from "Swerve Drive Second Order Kinematics"
    // by FRC Team 449 - The Blair Robot Project, Rafi Pedersen
    // https://www.chiefdelphi.com/uploads/short-url/qzj4k2LyBs7rLxAem0YajNIlStH.pdf

    var moduleAccelerations = new SwerveModuleAcceleration[m_numModules];

    if (chassisAccelerations.ax == 0.0
        && chassisAccelerations.ay == 0.0
        && chassisAccelerations.alpha == 0.0) {
      for (int i = 0; i < m_numModules; i++) {
        moduleAccelerations[i] = new SwerveModuleAcceleration(0.0, Rotation2d.kZero);
      }
      return moduleAccelerations;
    }

    if (!centerOfRotation.equals(m_prevCoR)) {
      setInverseKinematics(centerOfRotation);
    }

    var chassisAccelerationsVector = new SimpleMatrix(4, 1);
    chassisAccelerationsVector.setColumn(
        0,
        0,
        chassisAccelerations.ax,
        chassisAccelerations.ay,
        angularVelocity * angularVelocity,
        chassisAccelerations.alpha);

    var moduleAccelerationsMatrix = m_secondOrderInverseKinematics.mult(chassisAccelerationsVector);

    for (int i = 0; i < m_numModules; i++) {
      double x = moduleAccelerationsMatrix.get(i * 2, 0);
      double y = moduleAccelerationsMatrix.get(i * 2 + 1, 0);

      // For swerve modules, we need to compute both linear acceleration and angular acceleration
      // The linear acceleration is the magnitude of the acceleration vector
      double linearAcceleration = Math.hypot(x, y);

      if (linearAcceleration <= 1e-6) {
        moduleAccelerations[i] = new SwerveModuleAcceleration(linearAcceleration, Rotation2d.kZero);
      } else {
        moduleAccelerations[i] =
            new SwerveModuleAcceleration(linearAcceleration, new Rotation2d(x, y));
      }
    }

    return moduleAccelerations;
  }

  /**
   * Performs inverse kinematics. See {@link #toSwerveModuleAccelerations(ChassisAccelerations,
   * double, Translation2d)} toSwerveModuleAccelerations for more information.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @param angularVelocity The desired robot angular velocity.
   * @return An array containing the module accelerations.
   */
  public SwerveModuleAcceleration[] toSwerveModuleAccelerations(
      ChassisAccelerations chassisAccelerations, double angularVelocity) {
    return toSwerveModuleAccelerations(chassisAccelerations, angularVelocity, Translation2d.kZero);
  }

  @Override
  public SwerveModuleAcceleration[] toWheelAccelerations(
      ChassisAccelerations chassisAccelerations) {
    return toSwerveModuleAccelerations(chassisAccelerations, 0.0);
  }

  /**
   * Performs forward kinematics to return the resulting chassis accelerations from the given module
   * accelerations. This method is often used for dynamics calculations -- determining the robot's
   * acceleration on the field using data from the real-world acceleration of each module on the
   * robot.
   *
   * @param moduleAccelerations The accelerations of the modules as measured from respective
   *     encoders and gyros. The order of the swerve module accelerations should be same as passed
   *     into the constructor of this class.
   * @return The resulting chassis accelerations.
   */
  @Override
  public ChassisAccelerations toChassisAccelerations(
      SwerveModuleAcceleration... moduleAccelerations) {
    // Derivation for second-order kinematics from "Swerve Drive Second Order Kinematics"
    // by FRC Team 449 - The Blair Robot Project, Rafi Pedersen
    // https://www.chiefdelphi.com/uploads/short-url/qzj4k2LyBs7rLxAem0YajNIlStH.pdf

    if (moduleAccelerations.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of module locations provided in "
              + "constructor");
    }
    var moduleAccelerationsMatrix = new SimpleMatrix(m_numModules * 2, 1);

    for (int i = 0; i < m_numModules; i++) {
      var module = moduleAccelerations[i];

      moduleAccelerationsMatrix.set(i * 2 + 0, 0, module.acceleration * module.angle.getCos());
      moduleAccelerationsMatrix.set(i * 2 + 1, 0, module.acceleration * module.angle.getSin());
    }

    var chassisAccelerationsVector = m_secondOrderForwardKinematics.mult(moduleAccelerationsMatrix);

    // the second order kinematics equation for swerve drive yields a state vector [aₓ, a_y, ω², α]
    return new ChassisAccelerations(
        chassisAccelerationsVector.get(0, 0),
        chassisAccelerationsVector.get(1, 0),
        chassisAccelerationsVector.get(3, 0));
  }

  /**
   * Sets both inverse kinematics matrices based on the new center of rotation. This does not check
   * if the new center of rotation is different from the previous one, so a check should be included
   * before the call to this function.
   *
   * @param centerOfRotation new center of rotation
   */
  private void setInverseKinematics(Translation2d centerOfRotation) {
    for (int i = 0; i < m_numModules; i++) {
      var rx = m_modules[i].getX() - centerOfRotation.getX();
      var ry = m_modules[i].getY() - centerOfRotation.getY();

      m_firstOrderInverseKinematics.setRow(i * 2 + 0, 0, /* Start Data */ 1, 0, -ry);
      m_firstOrderInverseKinematics.setRow(i * 2 + 1, 0, /* Start Data */ 0, 1, rx);

      m_secondOrderInverseKinematics.setRow(i * 2 + 0, 0, /* Start Data */ 1, 0, -rx, -ry);
      m_secondOrderInverseKinematics.setRow(i * 2 + 1, 0, /* Start Data */ 0, 1, -ry, +rx);
    }
    m_prevCoR = centerOfRotation;
  }
}
