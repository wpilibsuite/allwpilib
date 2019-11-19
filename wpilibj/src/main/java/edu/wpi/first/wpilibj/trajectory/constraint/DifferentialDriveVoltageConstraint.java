/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
   * @param kinematics  A kinematics component describing the drive geometry.
   * @param maxVoltage  The maximum voltage available to the motors while following the path.
   *                    Should be somewhat less than the nominal battery voltage (12V) to account
   *                    for "voltage sag" due to current draw.
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
                                                                   velocityMetersPerSecond
                                                                       * curvatureRadPerMeter));

    double maxWheelSpeed = Math.max(wheelSpeeds.leftMetersPerSecond,
                                    wheelSpeeds.rightMetersPerSecond);
    double minWheelSpeed = Math.min(wheelSpeeds.leftMetersPerSecond,
                                    wheelSpeeds.rightMetersPerSecond);

    // Calculate maximum/minimum possible accelerations from motor dynamics
    // and max/min wheel speeds
    double maxWheelAcceleration =
        m_feedforward.maxAchievableAcceleration(m_maxVoltage, maxWheelSpeed);
    double minWheelAcceleration =
        m_feedforward.minAchievableAcceleration(m_maxVoltage, minWheelSpeed);

    // Robot chassis turning on radius = 1/|curvature|.  Outer wheel has radius
    // increased by half of the trackwidth T.  Inner wheel has radius decreased
    // by half of the trackwidth.  Achassis / radius = Aouter / (radius + T/2), so
    // Achassis = Aouter * radius / (radius + T/2) = Aouter / (1 + |curvature|T/2).
    // Inner wheel is similar.

    // sgn(speed) term added to correctly account for which wheel is on
    // outside of turn:
    // If moving forward, max acceleration constraint corresponds to wheel on outside of turn
    // If moving backward, max acceleration constraint corresponds to wheel on inside of turn

    double maxChassisAcceleration =
        maxWheelAcceleration
            / (1 + m_kinematics.trackWidthMeters * Math.abs(curvatureRadPerMeter)
            * Math.signum(velocityMetersPerSecond) / 2);
    double minChassisAcceleration =
        minWheelAcceleration
            / (1 - m_kinematics.trackWidthMeters * Math.abs(curvatureRadPerMeter)
            * Math.signum(velocityMetersPerSecond) / 2);

    // Negate acceleration of wheel on inside of turn if center of turn is inside of wheelbase
    if ((m_kinematics.trackWidthMeters / 2) > (1 / Math.abs(curvatureRadPerMeter))) {
      if (velocityMetersPerSecond > 0) {
        minChassisAcceleration = -minChassisAcceleration;
      } else {
        maxChassisAcceleration = -maxChassisAcceleration;
      }
    }

    return new MinMax(minChassisAcceleration, maxChassisAcceleration);
  }
}
