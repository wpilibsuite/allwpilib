package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.RamseteController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

import java.util.function.BiConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.Supplier;

/**
 * A command that uses a RAMSETE controller ({@link RamseteController}) to follow a trajectory
 * with a differential drive.
 *
 * <p>The command handles trajectory-following, PID calculations, and feedforwards internally.  This
 * is intended to be a more-or-less "complete solution" that can be used by teams without a great
 * deal of controls expertise - teams seeking more flexibility may want to write their own,
 * less-constrained version of this class.
 */
public class RamseteCommand extends CommandBase {
  private final Timer m_timer = new Timer();
  private DifferentialDriveWheelSpeeds m_prevSpeeds = new DifferentialDriveWheelSpeeds();
  private double m_prevTime;

  private final Trajectory m_trajectory;
  private final Supplier<Pose2d> m_pose;
  private final RamseteController m_follower;
  private final double m_ks;
  private final double m_kv;
  private final double m_ka;
  private final DifferentialDriveKinematics m_kinematics;
  private final DoubleSupplier m_leftSpeed;
  private final DoubleSupplier m_rightSpeed;
  private final PIDController m_leftController;
  private final PIDController m_rightController;
  private final BiConsumer<Double, Double> m_output;

  /**
   * Constructs a new RamseteCommand that, when executed, will follow the provided trajectory.
   *
   * @param trajectory                     The trajectory to follow.
   * @param pose                           A function that supplies the robot pose - use one of
   *                                       the odometry classes to provide this.
   * @param controller                     The RAMSETE controller used to follow the trajectory -
   *                                      see {@link RamseteController}.
   * @param ksVolts                        Constant feedforward term for the robot drive.
   * @param kvVoltSecondsPerMeter          Velocity-proportional feedforward term for the robot
   *                                       drive.
   * @param kaVoltSecondsSquaredPerMeter   Acceleration-proportional feedforward term for the robot
   *                                       drive.
   * @param trackwidthMeters               The trackwidth of the robot drive.  Should be
   *                                       empirically measured.
   * @param leftWheelSpeedMetersPerSecond  A function that supplies the speed of the left side of
   *                                       the robot drive.
   * @param rightWheelSpeedMetersPerSecond A function that supplies the speed of the right side
   *                                       of the robot drive.
   * @param leftController                 The PIDController for the left side of the robot drive.
   * @param rightController                The PIDController for the right side of the robot drive.
   * @param output                         A function that consumes the computed left and right
   *                                       outputs (scaled from -1 to 1) for the robot drive.
   */
  public RamseteCommand(Trajectory trajectory,
                        Supplier<Pose2d> pose,
                        RamseteController controller,
                        double ksVolts,
                        double kvVoltSecondsPerMeter,
                        double kaVoltSecondsSquaredPerMeter,
                        double trackwidthMeters,
                        DoubleSupplier leftWheelSpeedMetersPerSecond,
                        DoubleSupplier rightWheelSpeedMetersPerSecond,
                        PIDController leftController,
                        PIDController rightController,
                        BiConsumer<Double, Double> output) {
    m_trajectory = trajectory;
    m_pose = pose;
    m_follower = controller;
    m_ks = ksVolts;
    m_kv = kvVoltSecondsPerMeter;
    m_ka = kaVoltSecondsSquaredPerMeter;
    m_kinematics = new DifferentialDriveKinematics(trackwidthMeters);
    m_leftSpeed = leftWheelSpeedMetersPerSecond;
    m_rightSpeed = rightWheelSpeedMetersPerSecond;
    m_leftController = leftController;
    m_rightController = rightController;
    m_output = output;
  }

  @Override
  public void initialize() {
    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void execute() {
    double curTime = m_timer.get();
    double dt = curTime - m_prevTime;

    var wheelSpeeds = m_kinematics.toWheelSpeeds(
        m_follower.calculate(m_pose.get(), m_trajectory.sample(curTime)));

    double leftFeedforward = m_ks * Math.signum(wheelSpeeds.leftMetersPerSecond)
                             + m_kv * wheelSpeeds.leftMetersPerSecond
                             + m_ka * (wheelSpeeds.leftMetersPerSecond
                                       - m_prevSpeeds.leftMetersPerSecond) / dt;

    double rightFeedforward = m_ks * Math.signum(wheelSpeeds.rightMetersPerSecond)
                              + m_kv * wheelSpeeds.rightMetersPerSecond
                              + m_ka * (wheelSpeeds.rightMetersPerSecond
                                        - m_prevSpeeds.rightMetersPerSecond) / dt;

    double leftOutput = leftFeedforward / 12.
                        + m_leftController.calculate(wheelSpeeds.leftMetersPerSecond,
                                                     m_leftSpeed.getAsDouble());

    double rightOutput = rightFeedforward / 12.
                         + m_rightController.calculate(wheelSpeeds.rightMetersPerSecond,
                                                       m_rightSpeed.getAsDouble());

    m_output.accept(leftOutput, rightOutput);

    m_prevTime = curTime;
    m_prevSpeeds = wheelSpeeds;
  }

  @Override
  public void end(boolean interrupted) {
    m_output.accept(0., 0.);
    m_timer.stop();
  }

  @Override
  public boolean isFinished() {
    return m_timer.get() >= m_trajectory.getTotalTimeSeconds();
  }
}
