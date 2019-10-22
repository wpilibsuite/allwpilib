/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.function.Consumer;
import java.util.function.Supplier;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.controller.RamseteController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

/**
 * A command that uses a RAMSETE controller ({@link RamseteController}) to follow a trajectory
 * {@link Trajectory} with a differential drive.
 *
 * <p>The command handles trajectory-following, PID calculations, and feedforwards internally.  This
 * is intended to be a more-or-less "complete solution" that can be used by teams without a great
 * deal of controls expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to use the onboard
 * PID functionality of a "smart" motor controller) may use the secondary constructor that omits
 * the PID and feedforward functionality, returning only the raw wheel speeds from the RAMSETE
 * controller.
 */

public class MecanumFollowerCommand extends CommandBase {
  private final Timer m_timer = new Timer();
  private MecanumDriveWheelSpeeds m_prevSpeeds;
  private double m_prevTime;
  private Pose2d m_finalPose;

  private final Trajectory m_trajectory;
  private final Supplier<Pose2d> m_pose;
  private final double m_ks;
  private final double m_kv;
  private final double m_ka;
  private final MecanumDriveKinematics m_kinematics;
  private final PIDController m_xController;
  private final PIDController m_yController;
  private final ProfiledPIDController m_thetaController;
  private final PIDController m_frontLeftController;
  private final PIDController m_rearLeftController;
  private final PIDController m_frontRightController;
  private final PIDController m_rearRightController;
  private final Supplier<MecanumDriveWheelSpeeds> m_currentWheelSpeeds;
  private final Consumer<Double> m_frontLeftOutput;
  private final Consumer<Double> m_rearLeftOutput;
  private final Consumer<Double> m_frontRightOutput;
  private final Consumer<Double> m_rearRightOutput;

  @SuppressWarnings("PMD.ExcessiveParameterList")
  public MecanumFollowerCommand(Trajectory trajectory,
                        Supplier<Pose2d> pose,
                        double ksVolts,
                        double kvVoltSecondsPerMeter,
                        double kaVoltSecondsSquaredPerMeter,
                        MecanumDriveKinematics kinematics,

                        PIDController xController,
                        PIDController yController,
                        ProfiledPIDController thetaController,

                        PIDController frontLeftController,
                        PIDController rearLeftController,
                        PIDController frontRightController,
                        PIDController rearRightController,

                        Supplier<MecanumDriveWheelSpeeds> currentWheelSpeeds,

                        Consumer<Double> frontLeftOutputVolts,
                        Consumer<Double> rearLeftOutputVolts,
                        Consumer<Double> frontRightOutputVolts,
                        Consumer<Double> rearRightOutputVolts,
                        Subsystem... requirements) {
    m_trajectory = requireNonNullParam(trajectory, "trajectory", "MecanumFollowerCommand");
    m_pose = requireNonNullParam(pose, "pose", "MecanumFollowerCommand");
    m_ks = ksVolts;
    m_kv = kvVoltSecondsPerMeter;
    m_ka = kaVoltSecondsSquaredPerMeter;
    m_kinematics = requireNonNullParam(kinematics, "kinematics", "MecanumFollowerCommand");

    m_xController = requireNonNullParam(xController, "xController", "MecanumFollowerCommand");
    m_yController = requireNonNullParam(yController, "yController", "MecanumFollowerCommand");
    m_thetaController = requireNonNullParam(thetaController, "thetaController", "MecanumFollowerCommand");

    m_frontLeftController = requireNonNullParam(frontLeftController, "frontLeftController", "MecanumFollowerCommand");
    m_rearLeftController = requireNonNullParam(rearLeftController, "rearLeftController", "MecanumFollowerCommand");
    m_frontRightController = requireNonNullParam(frontRightController, "frontRightController", "MecanumFollowerCommand");
    m_rearRightController = requireNonNullParam(rearRightController, "rearRightController", "MecanumFollowerCommand");

    m_currentWheelSpeeds = requireNonNullParam(currentWheelSpeeds, "currentWheelSpeeds", "MecanumFollowerCommand");

    m_frontLeftOutput = requireNonNullParam(frontLeftOutputVolts, "frontLeftOutput", "MecanumFollowerCommand");
    m_rearLeftOutput = requireNonNullParam(rearLeftOutputVolts, "rearLeftOutput", "MecanumFollowerCommand");
    m_frontRightOutput =  requireNonNullParam(frontRightOutputVolts, "frontRightOutput", "MecanumFollowerCommand");
    m_rearRightOutput = requireNonNullParam(rearRightOutputVolts, "rearRightOutput", "MecanumFollowerCommand");
    addRequirements(requirements);
  }

  public MecanumFollowerCommand(Trajectory trajectory,
                        Supplier<Pose2d> pose,
                        MecanumDriveKinematics kinematics,
                        PIDController xController,
                        PIDController yController,
                        ProfiledPIDController thetaController,

                        Supplier<MecanumDriveWheelSpeeds> currentWheelSpeeds,

                        Consumer<Double> frontLeftOutputMetersPerSecond,
                        Consumer<Double> rearLeftOutputMetersPerSecond,
                        Consumer<Double> frontRightOutputMetersPerSecond,
                        Consumer<Double> rearRightOutputMetersPerSecond,
                        Subsystem... requirements) {
    m_trajectory = requireNonNullParam(trajectory, "trajectory", "MecanumFollowerCommand");
    m_pose = requireNonNullParam(pose, "pose", "MecanumFollowerCommand");
    m_ks = 0;
    m_kv = 0;
    m_ka = 0;
    m_kinematics = requireNonNullParam(kinematics, "kinematics", "MecanumFollowerCommand");

    m_xController = requireNonNullParam(xController, "xController", "MecanumFollowerCommand");
    m_yController = requireNonNullParam(yController, "xController", "MecanumFollowerCommand");
    m_thetaController = requireNonNullParam(thetaController, "thetaController", "MecanumFollowerCommand");

    m_frontLeftController = null;
    m_rearLeftController = null;
    m_frontRightController = null;
    m_rearRightController = null;

    m_currentWheelSpeeds = requireNonNullParam(currentWheelSpeeds, "currentWheelSpeeds", "MecanumFollowerCommand");

    m_frontLeftOutput = requireNonNullParam(frontLeftOutputMetersPerSecond, "frontLeftOutput", "MecanumFollowerCommand");
    m_rearLeftOutput = requireNonNullParam(rearLeftOutputMetersPerSecond, "rearLeftOutput", "MecanumFollowerCommand");
    m_frontRightOutput =  requireNonNullParam(frontRightOutputMetersPerSecond, "frontRightOutput", "MecanumFollowerCommand");
    m_rearRightOutput = requireNonNullParam(rearRightOutputMetersPerSecond, "rearRightOutput", "MecanumFollowerCommand");
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    var initialState = m_trajectory.sample(0);
    m_finalPose = m_trajectory.sample(m_trajectory.getTotalTimeSeconds()).poseMeters;

    var initialXVelocity = initialState.velocityMetersPerSecond * Math.sin(initialState.poseMeters.getRotation().getRadians());
    var initialYVelocity = initialState.velocityMetersPerSecond * Math.cos(initialState.poseMeters.getRotation().getRadians());

    m_prevSpeeds = m_kinematics.toWheelSpeeds( new ChassisSpeeds(initialXVelocity, initialYVelocity, initialState.curvatureRadPerMeter * initialState.velocityMetersPerSecond));

    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void execute() {
    double curTime = m_timer.get();
    double dt = curTime - m_prevTime;

    var m_desiredState = m_trajectory.sample(curTime);
    var m_desiredPose = m_desiredState.poseMeters;

    var m_poseError = m_desiredPose.relativeTo(m_pose.get());

    double targetXVel = m_xController.calculate(
      m_pose.get().getTranslation().getX(),
       m_desiredPose.getTranslation().getX());

    double targetYVel = m_yController.calculate(
      m_pose.get().getTranslation().getY(),
       m_desiredPose.getTranslation().getY());
       
    double targetAngularVel = m_thetaController.calculate(
      m_pose.get().getRotation().getRadians(),
       m_finalPose.getRotation().getRadians());

    double vRef = m_desiredState.velocityMetersPerSecond;

    targetXVel += vRef * Math.sin(m_poseError.getRotation().getRadians());
    targetYVel += vRef * Math.cos(m_poseError.getRotation().getRadians());

    var targetChassisSpeeds = new ChassisSpeeds(targetXVel, targetYVel, targetAngularVel);

    var targetWheelSpeeds = m_kinematics.toWheelSpeeds(targetChassisSpeeds);

    var frontLeftSpeedSetpoint = targetWheelSpeeds.frontLeftMetersPerSecond;
    var rearLeftSpeedSetpoint = targetWheelSpeeds.rearLeftMetersPerSecond;
    var frontRightSpeedSetpoint = targetWheelSpeeds.frontRightMetersPerSecond;
    var rearRightSpeedSetpoint =  targetWheelSpeeds.rearRightMetersPerSecond;

    double frontLeftOutput;
    double rearLeftOutput;
    double frontRightOutput;
    double rearRightOutput;

    if (m_frontLeftController != null) {
      double frontLeftFeedforward =
          m_ks * Math.signum(frontLeftSpeedSetpoint)
              + m_kv * frontLeftSpeedSetpoint
              + m_ka * (frontLeftSpeedSetpoint - m_prevSpeeds.frontLeftMetersPerSecond) / dt;

      double rearLeftFeedforward =
          m_ks * Math.signum(rearLeftSpeedSetpoint)
              + m_kv * rearLeftSpeedSetpoint
              + m_ka * (rearLeftSpeedSetpoint - m_prevSpeeds.rearLeftMetersPerSecond) / dt;

      double frontRightFeedforward =
          m_ks * Math.signum(frontRightSpeedSetpoint)
              + m_kv * frontRightSpeedSetpoint
              + m_ka * (frontRightSpeedSetpoint - m_prevSpeeds.frontRightMetersPerSecond) / dt;

      double rearRightFeedforward =
                m_ks * Math.signum(rearRightSpeedSetpoint)
                    + m_kv * rearRightSpeedSetpoint
                    + m_ka * (rearRightSpeedSetpoint - m_prevSpeeds.rearRightMetersPerSecond) / dt;


      frontLeftOutput = frontLeftFeedforward
          + m_frontLeftController.calculate(
            m_currentWheelSpeeds.get().frontLeftMetersPerSecond,
             frontLeftSpeedSetpoint);

      rearLeftOutput = rearLeftFeedforward
          + m_rearLeftController.calculate(
            m_currentWheelSpeeds.get().rearLeftMetersPerSecond,
             rearLeftSpeedSetpoint);

      frontRightOutput = frontRightFeedforward
          + m_frontRightController.calculate(
            m_currentWheelSpeeds.get().frontRightMetersPerSecond,
             frontRightSpeedSetpoint);

      rearRightOutput = rearRightFeedforward
          + m_rearRightController.calculate(
            m_currentWheelSpeeds.get().rearRightMetersPerSecond,
             rearRightSpeedSetpoint);


    } else {
      frontLeftOutput = frontLeftSpeedSetpoint;
      rearLeftOutput = rearLeftSpeedSetpoint;
      frontRightOutput = frontRightSpeedSetpoint;
      rearRightOutput = rearRightSpeedSetpoint;
    }

    m_frontLeftOutput.accept(frontLeftOutput);
    m_rearLeftOutput.accept(rearLeftOutput);
    m_frontRightOutput.accept(frontRightOutput);
    m_rearRightOutput.accept(rearRightOutput);

    m_prevTime = curTime;
    m_prevSpeeds = targetWheelSpeeds;
  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();
  }

  @Override
  public boolean isFinished() {
    return m_timer.get() - m_trajectory.getTotalTimeSeconds() >= 0;
  }
}
