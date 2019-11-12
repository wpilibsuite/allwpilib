package edu.wpi.first.wpilibj.trajectory.constraint;

import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A class that enforces constraints on differential drive voltage expenditure based on the motor
 * dynamics and the drive kinematics.  Ensures that the acceleration of any wheel of the robot
 * while following the trajectory is never higher than what can be achieved with the given
 * maximum voltage.
 */
public class DifferentialDriveVoltageConstraint implements TrajectoryConstraint {
  private final SimpleMotorFeedforward m_feedforward;
  private final DifferentialDriveKinematics m_kinematics;
  private final double m_maxVoltage;

  /**
   * Creates a new DifferentialDriveVoltageConstraint.
   *
   * @param feedforward A feedforward component describing the behavior of the drive.
   * @param kinematics A kinematics component describing the drive geometry.
   * @param maxVoltage The maximum voltage available to the motors while following the path.
   *                   Should be somewhat less than the nominal battery voltage (12V) to account
   *                   for "voltage sag" due to current draw.
   */
  public DifferentialDriveVoltageConstraint(SimpleMotorFeedforward feedforward,
                                            DifferentialDriveKinematics kinematics,
                                            double maxVoltage) {
    m_feedforward = requireNonNullParam(feedforward, "feedforward",
                                        "DifferentialDriveVoltageConstraint");
    m_kinematics = requireNonNullParam(kinematics, "kinematics",
                                       "DifferentialDriveVoltageConstraint");
    m_maxVoltage = maxVoltage;
  }

  @Override
  public double getMaxVelocityMetersPerSecond(Pose2d poseMeters, double curvatureRadPerMeter,
                                              double velocityMetersPerSecond) {
    return Double.POSITIVE_INFINITY;
  }

  @Override
  public MinMax getMinMaxAccelerationMetersPerSecondSq(Pose2d poseMeters,
                                                       double curvatureRadPerMeter,
                                                       double velocityMetersPerSecond) {
    var wheelSpeeds = m_kinematics.toWheelSpeeds(new ChassisSpeeds(velocityMetersPerSecond, 0,
                                                                   velocityMetersPerSecond *
                                                                       curvatureRadPerMeter));
    double maxWheelSpeed = Math.max(Math.abs(wheelSpeeds.leftMetersPerSecond),
                                    Math.abs(wheelSpeeds.rightMetersPerSecond));
    double maxWheelAcceleration =
        (m_maxVoltage - m_feedforward.ks - m_feedforward.kv * maxWheelSpeed) / m_feedforward.ka;
    double maxChassisAcceleration =
        maxWheelAcceleration / (1 + m_kinematics.trackWidthMeters * curvatureRadPerMeter / 2);
    return new MinMax(-maxChassisAcceleration, maxChassisAcceleration);
  }
}
