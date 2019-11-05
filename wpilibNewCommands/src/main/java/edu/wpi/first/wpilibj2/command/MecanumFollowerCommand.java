/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.function.DoubleConsumer;
import java.util.function.Supplier;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

/**
 * A command that uses two PID controllers ({@link PIDController}) and a ProfiledPIDController ({@link ProfiledPIDController}) to follow a trajectory
 * {@link Trajectory} with a mecanum drive.
 *
 * <p>The command handles trajectory-following, Velocity PID calculations, and feedforwards internally. This
 * is intended to be a more-or-less "complete solution" that can be used by teams without a great
 * deal of controls expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to use the onboard
 * PID functionality of a "smart" motor controller) may use the secondary constructor that omits
 * the PID and feedforward functionality, returning only the raw wheel speeds from the RAMSETE
 * controller.
 *
 * <p>The robot angle controller does not follow the angle given by
 * the trajectory but rather goes to the angle given in the final state of the trajectory.
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
  private final double m_maxWheelVelocityMetersPerSecond;
  private final PIDController m_frontLeftController;
  private final PIDController m_rearLeftController;
  private final PIDController m_frontRightController;
  private final PIDController m_rearRightController;
  private final Supplier<MecanumDriveWheelSpeeds> m_currentWheelSpeeds;
  private final DoubleConsumer m_frontLeftOutput;
  private final DoubleConsumer m_rearLeftOutput;
  private final DoubleConsumer m_frontRightOutput;
  private final DoubleConsumer m_rearRightOutput;

  /**
   * Constructs a new MecanumFollowerCommand that, when executed, will follow the provided trajectory.
   * PID control and feedforward are handled internally, outputs are scaled from -12 to 12 as a voltage output to the motor.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path -
   * this
   * is left to the user, since it is not appropriate for paths with nonstationary endstates.
   * 
   * <p>Note2: The rotation controller will calculate the rotation based on the final pose in the trajectory, not the poses at each time step.
   *
   * @param trajectory                        The trajectory to follow.
   * @param pose                              A function that supplies the robot pose - use one of
   *                                          the odometry classes to provide this.
   * @param ksVolts                           Constant feedforward term for the robot drive.
   * @param kvVoltSecondsPerMeter             Velocity-proportional feedforward term for the robot
   *                                          drive.
   * @param kaVoltSecondsSquaredPerMeter      Acceleration-proportional feedforward term for the robot
   *                                          drive.
   * @param kinematics                        The kinematics for the robot drivetrain.
   * @param xController                       The Trajectory Tracker PID controller for the robot's x position.
   * @param yController                       The Trajectory Tracker PID controller for the robot's y position.
   * @param thetaController                   The Trajectory Tracker PID controller for angle for the robot.
   * @param maxWheelVelocityMetersPerSecond   The maximum velocity of a drivetrain wheel.
   * @param frontLeftController               The front left wheel velocity PID.
   * @param rearLeftController                The rear left wheel velocity PID.
   * @param frontRightController              The front right wheel velocity PID.
   * @param rearRightController               The rear right wheel velocity PID.
   * @param currentWheelSpeeds                A MecanumDriveWheelSpeeds object containing the current wheel speeds.
   * @param frontLeftOutputVolts              The front left wheel output in volts.
   * @param rearLeftOutputVolts               The rear left wheel output in volts.`
   * @param frontRightOutputVolts             The front right wheel output in volts.
   * @param rearRightOutputVolts              The rear right wheel output in volts.
   * @param requirements                      The subsystems to require.
   */

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

                        double maxWheelVelocityMetersPerSecond,

                        PIDController frontLeftController,
                        PIDController rearLeftController,
                        PIDController frontRightController,
                        PIDController rearRightController,

                        Supplier<MecanumDriveWheelSpeeds> currentWheelSpeeds,

                        DoubleConsumer frontLeftOutputVolts,
                        DoubleConsumer rearLeftOutputVolts,
                        DoubleConsumer frontRightOutputVolts,
                        DoubleConsumer rearRightOutputVolts,
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

    m_maxWheelVelocityMetersPerSecond = requireNonNullParam(maxWheelVelocityMetersPerSecond, "maxWheelVelocityMetersPerSecond", "MecanumFollowerCommand");

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

  /**
   * Constructs a new MecanumFollowerCommand that, when executed, will follow the provided trajectory.
   * The user should implement a velocity PID on the desired output wheel velocities.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path -
   * this
   * is left to the user, since it is not appropriate for paths with non-stationary end-states.
   * 
   * <p>Note2: The rotation controller will calculate the rotation based on the final pose in the trajectory, not the poses at each time step.
   *
   * @param trajectory                        The trajectory to follow.
   * @param pose                              A function that supplies the robot pose - use one of
   *                                          the odometry classes to provide this.
   * @param kinematics                        The kinematics for the robot drivetrain.
   * @param xController                       The Trajectory Tracker PID controller for the robot's x position.
   * @param yController                       The Trajectory Tracker PID controller for the robot's y position.
   * @param thetaController                   The Trajectory Tracker PID controller for angle for the robot.
   * @param maxWheelVelocityMetersPerSecond   The maximum velocity of a drivetrain wheel.
   * @param frontLeftOutputMetersPerSecond    The front left wheel output in volts.
   * @param rearLeftOutputMetersPerSecond     The rear left wheel output in volts.`
   * @param frontRightOutputMetersPerSecond   The front right wheel output in volts.
   * @param rearRightOutputMetersPerSecond    The rear right wheel output in volts.
   * @param requirements                      The subsystems to require.
   */

  public MecanumFollowerCommand(Trajectory trajectory,
                        Supplier<Pose2d> pose,
                        MecanumDriveKinematics kinematics,
                        PIDController xController,
                        PIDController yController,
                        ProfiledPIDController thetaController,

                        double maxWheelVelocityMetersPerSecond,

                        DoubleConsumer frontLeftOutputMetersPerSecond,
                        DoubleConsumer rearLeftOutputMetersPerSecond,
                        DoubleConsumer frontRightOutputMetersPerSecond,
                        DoubleConsumer rearRightOutputMetersPerSecond,
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

    m_maxWheelVelocityMetersPerSecond = requireNonNullParam(maxWheelVelocityMetersPerSecond, "maxWheelVelocityMetersPerSecond", "MecanumFollowerCommand");

    m_frontLeftController = null;
    m_rearLeftController = null;
    m_frontRightController = null;
    m_rearRightController = null;

    m_currentWheelSpeeds = null;

    m_frontLeftOutput = requireNonNullParam(frontLeftOutputMetersPerSecond, "frontLeftOutput", "MecanumFollowerCommand");
    m_rearLeftOutput = requireNonNullParam(rearLeftOutputMetersPerSecond, "rearLeftOutput", "MecanumFollowerCommand");
    m_frontRightOutput =  requireNonNullParam(frontRightOutputMetersPerSecond, "frontRightOutput", "MecanumFollowerCommand");
    m_rearRightOutput = requireNonNullParam(rearRightOutputMetersPerSecond, "rearRightOutput", "MecanumFollowerCommand");
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    var initialState = m_trajectory.sample(0);
    m_finalPose = m_trajectory.sample(m_trajectory.getTotalTimeSeconds()).poseMeters; //Sample final pose to get robot rotation

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
       // The robot will go to the desired rotation of the final pose in the trajectory,
       // not following the poses at individual states.

    double vRef = m_desiredState.velocityMetersPerSecond;

    targetXVel += vRef * Math.sin(m_poseError.getRotation().getRadians());
    targetYVel += vRef * Math.cos(m_poseError.getRotation().getRadians());

    var targetChassisSpeeds = new ChassisSpeeds(targetXVel, targetYVel, targetAngularVel);

    var targetWheelSpeeds = m_kinematics.toWheelSpeeds(targetChassisSpeeds);

    targetWheelSpeeds.normalize(m_maxWheelVelocityMetersPerSecond);

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
    return m_timer.hasPeriodPassed(m_trajectory.getTotalTimeSeconds());
  }
}