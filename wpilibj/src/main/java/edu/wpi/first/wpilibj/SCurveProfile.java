/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Provides trapezoidal acceleration control.
 *
 * <p>Constant acceleration until target (max) velocity is reached, sets acceleration to zero for a
 * calculated time, then decelerates at a constant acceleration with a slope equal to the negative
 * slope of the initial acceleration.
 */
public class SCurveProfile extends MotionProfile {
  private double m_acceleration;
  private double m_maxVelocity;
  private double m_profileMaxVelocity;
  private double m_timeToMaxA;

  private double m_jerk;
  private double m_t2;
  private double m_t3;
  private double m_t4;
  private double m_t5;
  private double m_t6;
  private double m_t7;

  private double m_sign;

  /**
   * Constructs an SCurveProfile.
   *
   * @param output     The output to update.
   * @param maxV       Maximum velocity.
   * @param maxA       Maximum acceleration.
   * @param timeToMaxA Time to maximum acceleration from no acceleration.
   * @param period     The period after which to update the output. The default is 50ms.
   */
  public SCurveProfile(PIDOutput output, double maxV, double maxA, double timeToMaxA,
      double period) {
    super(output, period);

    setMaxVelocity(maxV);
    setMaxAcceleration(maxA);
    setTimeToMaxA(timeToMaxA);
  }

  /**
   * Constructs an SCurveProfile.
   *
   * @param output     The output to update.
   * @param maxV       Maximum velocity.
   * @param maxA       Maximum acceleration.
   * @param timeToMaxA Time to maximum acceleration from no acceleration.
   */
  public SCurveProfile(PIDOutput output, double maxV, double maxA, double timeToMaxA) {
    this(output, maxV, maxA, timeToMaxA, 0.05);
  }

  /**
   * Sets goal state of profile.
   *
   * @param goal a distance to which to travel
   * @param currentSource the current position
   */
  public void setGoal(double goal, double currentSource) {
    m_mutex.lock();

    // Subtract current source for profile calculations
    m_goal = goal - currentSource;

    // Set setpoint to current distance since setpoint hasn't moved yet
    m_ref = new MotionProfile.State(currentSource, 0.0, 0.0);

    if (m_goal < 0.0) {
      m_sign = -1.0;
    } else {
      m_sign = 1.0;
    }

    // If profile can't accelerate up to max velocity before decelerating
    boolean shortProfile =
        m_maxVelocity * (m_timeToMaxA + m_maxVelocity / m_acceleration)
        > m_sign * m_goal;

    if (shortProfile) {
      m_profileMaxVelocity =
          m_acceleration * (Math.sqrt(m_sign * m_goal / m_acceleration
                                      - 0.75 * Math.pow(m_timeToMaxA, 2))
                            - 0.5 * m_timeToMaxA);
    } else {
      m_profileMaxVelocity = m_maxVelocity;
    }

    // Find times at critical points
    m_t2 = m_profileMaxVelocity / m_acceleration;
    m_t3 = m_t2 + m_timeToMaxA;
    if (shortProfile) {
      m_t4 = m_t3;
    } else {
      m_t4 = m_sign * m_goal / m_profileMaxVelocity;
    }
    m_t5 = m_t4 + m_timeToMaxA;
    m_t6 = m_t4 + m_t2;
    m_t7 = m_t6 + m_timeToMaxA;
    m_totalTime = m_t7;

    // Restore desired goal
    m_goal = goal;

    reset();

    m_mutex.unlock();
  }

  /**
   * Sets goal state of profile.
   *
   * @param goal a distance to which to travel
   */
  public void setGoal(double goal) {
    setGoal(goal, 0.0);
  }

  /**
   * Sets maximum velocity of profile.
   *
   * @param velocity maximum velocity
   */
  public void setMaxVelocity(double velocity) {
    m_mutex.lock();
    m_maxVelocity = velocity;
    m_mutex.unlock();
  }

  /**
   * Returns maximum velocity of profile.
   */
  public double getMaxVelocity() {
    m_mutex.lock();
    try {
      return m_maxVelocity;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Sets maximum acceleration of profile.
   *
   * @param acceleration maximum acceleration
   */
  public void setMaxAcceleration(double acceleration) {
    m_mutex.lock();
    m_acceleration = acceleration;
    m_jerk = m_acceleration / m_timeToMaxA;
    m_mutex.unlock();
  }

  /**
   * Sets time to maximum acceleration from no acceleration.
   *
   * @param timeToMaxA time to maximum acceleration
   */
  public void setTimeToMaxA(double timeToMaxA) {
    m_mutex.lock();
    m_timeToMaxA = timeToMaxA;
    m_jerk = m_acceleration / m_timeToMaxA;
    m_mutex.unlock();
  }

  @Override
  protected State updateReference(double currentTime) {
    m_mutex.lock();

    if (currentTime < m_timeToMaxA) {
      // Ramp up acceleration
      m_ref.m_acceleration = m_jerk * currentTime;
      m_ref.m_velocity = 0.5 * m_jerk * Math.pow(currentTime, 2);
    } else if (currentTime < m_t2) {
      // Increase speed at max acceleration
      m_ref.m_acceleration = m_acceleration;
      m_ref.m_velocity = m_acceleration * (currentTime - 0.5 * m_timeToMaxA);
    } else if (currentTime < m_t3) {
      // Ramp down acceleration
      m_ref.m_acceleration = m_acceleration - m_jerk * (m_t2 - currentTime);
      m_ref.m_velocity =
          m_acceleration * m_t2 - 0.5 * m_jerk * Math.pow(m_t2 - currentTime, 2);
    } else if (currentTime < m_t4) {
      // Maintain max velocity
      m_ref.m_acceleration = 0.0;
      m_ref.m_velocity = m_profileMaxVelocity;
    } else if (currentTime < m_t5) {
      // Ramp down acceleration
      m_ref.m_acceleration = -m_jerk * (currentTime - m_t4);
      m_ref.m_velocity =
          m_profileMaxVelocity - 0.5 * m_jerk * Math.pow(currentTime - m_t4, 2);
    } else if (currentTime < m_t6) {
      // Decrease speed at max acceleration
      m_ref.m_acceleration = m_acceleration;
      m_ref.m_velocity = m_acceleration * (m_t2 + m_t5 - currentTime);
    } else if (currentTime < m_t7) {
      // Ramp up acceleration
      m_ref.m_acceleration = m_jerk * (m_t6 - currentTime);
      m_ref.m_velocity = 0.5 * m_jerk * Math.pow(m_t6 - currentTime, 2);
    }

    if (currentTime < m_t7) {
      m_ref.m_displacement += m_sign * m_ref.m_velocity * (currentTime - m_lastTime);
      m_lastTime = currentTime;
    }

    try {
      return m_ref;
    } finally {
      m_mutex.unlock();
    }
  }
}
