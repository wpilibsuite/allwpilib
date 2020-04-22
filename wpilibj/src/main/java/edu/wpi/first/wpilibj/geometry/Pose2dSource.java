/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

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
public class Pose2dSource implements Sendable, Supplier<Pose2d> {
  private final Supplier<Pose2d> m_source;

  /**
   * Creates a {@code Pose2dSource} that wraps the given source.
   * @param source the source of the poses, i.e the odometry's
   * {@link edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry#getPoseMeters() getPoseMeters()} method.
   * @param name the name to associate with this Pose2dSource in LiveWindow.
   Defaults to {@code "Robot Pose"}.
   */
  public Pose2dSource(Supplier<Pose2d> source, String name) {
    m_source = source;

    SendableRegistry.addLW(this, name);
  }

  /**
   * Creates a {@code Pose2dSource} that wraps the given source.
   * @param source the source of the poses, i.e the odometry's
   * {@link edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry#getPoseMeters() getPoseMeters()} method.
   */
  public Pose2dSource(Supplier<Pose2d> source) {
    this(source, "Robot Pose");
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setActuator(false);
    builder.addDoubleProperty("x_meters", () -> get().getTranslation().getX(), null);
    builder.addDoubleProperty("y_meters", () -> get().getTranslation().getY(), null);
    builder.addDoubleProperty("rotation_degrees", () -> get().getRotation().getDegrees(), null);
  }

  @Override
  public Pose2d get() {
    return m_source.get();
  }
}
