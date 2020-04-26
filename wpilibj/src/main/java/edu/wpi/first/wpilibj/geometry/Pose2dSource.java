/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import java.util.Locale;
import java.util.function.Supplier;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * A wrapper class of a <code>{@literal Supplier<Pose2d>}</code> and implements the {@code Sendable}
 * interface, so the updated pose can be displayed on LiveWindow.
 *
 * <p>Can also be used as a <code>{@literal Supplier<Pose2d>}</code> in the RamseteCommand
 * constructor.
 */
public class Pose2dSource implements Sendable, Supplier<Pose2d>, AutoCloseable {
  private final AngleMeasurement m_angleMeasurement;
  private final DistanceMeasurement m_distanceMeasurement;
  private final Supplier<Pose2d> m_source;

  /**
   * Creates a {@code Pose2dSource} that wraps the given source.
   * @param source the source of the poses, i.e the odometry's {@link
   *  edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry#getPoseMeters() getPose()} method.
   * @param name the name to associate with this Pose2dSource in LiveWindow.
   Defaults to {@code "Robot Pose"}.
   */
  public Pose2dSource(Supplier<Pose2d> source, String name) {
    this(source, name, AngleMeasurement.kDegrees);
  }

  /**
   * Creates a {@code Pose2dSource} that wraps the given source.
   * @param source the source of the poses, i.e the odometry's {@link
   *  edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry#getPoseMeters() getPose()} method.
   * @param name the name to associate with this Pose2dSource in LiveWindow.
   Defaults to {@code "Robot Pose"}.
   * @param distanceMeasurement the units that the pose is shown in LiveWindow (inches/meters).
   * @param angleMeasurement the units that the rotation is shown in LiveWindow (degrees/radians).
   */
  public Pose2dSource(Supplier<Pose2d> source, String name, DistanceMeasurement distanceMeasurement,
                      AngleMeasurement angleMeasurement) {
    m_source = source;
    m_distanceMeasurement = distanceMeasurement;
    m_angleMeasurement = angleMeasurement;

    SendableRegistry.addLW(this, name);
  }

  /**
   * Creates a {@code Pose2dSource} that wraps the given source.
   * @param source the source of the poses, i.e the odometry's {@link
   *  edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry#getPoseMeters() getPose()} method.
   */
  public Pose2dSource(Supplier<Pose2d> source) {
    this(source, "Robot Pose");
  }

  /**
   * Creates a {@code Pose2dSource} that wraps the given source.
   * @param source the source of the poses, i.e the odometry's {@link
   *  edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry#getPoseMeters() getPose()} method.
   * @param name the name to associate with this Pose2dSource in LiveWindow.
  Defaults to {@code "Robot Pose"}.
   * @param angleMeasurement the units that the rotation is shown in LiveWindow (degrees/radians).
   */
  public Pose2dSource(Supplier<Pose2d> source, String name, AngleMeasurement angleMeasurement) {
    this(source, name, DistanceMeasurement.kMeters, angleMeasurement);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setActuator(false);
    builder.addDoubleProperty(String.format("x_%s", m_distanceMeasurement),
        () -> get().getTranslation().getX(), null);
    builder.addDoubleProperty(String.format("y_%s", m_distanceMeasurement),
        () -> get().getTranslation().getY(), null);
    builder.addDoubleProperty(String.format("rotation_%s", m_angleMeasurement),
        () -> m_angleMeasurement.getAngle(get()), null);
  }

  @Override
  public Pose2d get() {
    return m_source.get();
  }

  @Override
  public void close() throws Exception {
    SendableRegistry.remove(this);
  }

  public enum AngleMeasurement {
    kRadians, kDegrees;

    @Override
    public String toString() {
      return name().substring(1).toLowerCase(Locale.ENGLISH);
    }

    private double getAngle(Pose2d pose) {
      Rotation2d rot = pose.getRotation();
      return this == kRadians ? rot.getRadians() : rot.getDegrees();
    }
  }

  public enum DistanceMeasurement {
    kInches, kMeters;

    @Override
    public String toString() {
      return name().substring(1).toLowerCase(Locale.ENGLISH);
    }
  }
}
