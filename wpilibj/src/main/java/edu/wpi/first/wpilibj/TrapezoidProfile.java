/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Provides trapezoidal velocity control.
 *
 * <p>Constant acceleration until target (max) velocity is reached, sets acceleration to zero for a
 * calculated time, then decelerates at a constant acceleration with a slope equal to the negative
 * slope of the initial acceleration.
 */
public class TrapezoidProfile extends MotionProfile {
  private double m_acceleration;
  private double m_velocity;
  private double m_profileMaxVelocity;
  private double m_timeFromMaxVelocity;
  private double m_timeToMaxVelocity;
  private double m_sign;

  /**
   * Constructs a TrapezoidProfile.
   *
   * @param output     The output to update.
   * @param maxV       Maximum velocity.
   * @param timeToMaxV Time to maximum velocity from a stop.
   * @param period     The period after which to update the output. The default is 50ms.
   */
  public TrapezoidProfile(PIDOutput output, double maxV, double timeToMaxV, double period) {
    super(output, period);

    setMaxVelocity(maxV);
    setTimeToMaxV(timeToMaxV);
  }

  /**
   * Constructs a TrapezoidProfile.
   *
   * @param output     The output to update.
   * @param maxV       Maximum velocity.
   * @param timeToMaxV Time to maximum velocity from a stop.
   */
  public TrapezoidProfile(PIDOutput output, double maxV, double timeToMaxV) {
    this(output, maxV, timeToMaxV, 0.05);
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
    m_timeToMaxVelocity = m_velocity / m_acceleration;

    /* d is distance traveled when accelerating to/from max velocity
     *       = 1/2 * (v0 + v) * t
     * t is m_timeToMaxVelocity
     * delta is distance traveled at max velocity
     * delta = totalDist - 2 * d
     *       = setpoint - 2 * ((v0 + v)/2 * t)
     * v0 = 0 therefore:
     * delta = setpoint - 2 * (v/2 * t)
     *       = setpoint - v * t
     *       = m_setpoint - m_velocity * m_timeToMaxVelocity
     *
     * t is time at maximum velocity
     * t = delta (from previous comment) / m_velocity (where m_velocity is maximum
     * velocity)
     *   = (m_setpoint - m_velocity * m_timeToMaxVelocity) / m_velocity
     *   = m_setpoint/m_velocity - m_timeToMaxVelocity
     */
    double timeAtMaxV = m_sign * m_goal / m_velocity - m_timeToMaxVelocity;

    /* if ( 1/2 * a * t^2 > m_setpoint / 2 ) // if distance travelled before
     *     reaching maximum speed is more than half of the total distance to
     *     travel
     * if ( a * t^2 > m_setpoint )
     * if ( a * (v/a)^2 > m_setpoint )
     * if ( a * v^2/a^2 > m_setpoint )
     * if ( v^2/a > m_setpoint )
     * if ( v * v/a > m_setpoint )
     * if ( v * m_timeToMaxVelocity > m_setpoint )
     */
    if (m_velocity * m_timeToMaxVelocity > m_sign * m_goal) {
      /* Solve for t:
       * 1/2 * a * t^2 = m_setpoint/2
       * a * t^2 = m_setpoint
       * t^2 = m_setpoint / a
       * t = Math.sqrt( m_setpoint / a )
       */
      m_timeToMaxVelocity = Math.sqrt(m_sign * m_goal / m_acceleration);
      m_timeFromMaxVelocity = m_timeToMaxVelocity;
      m_totalTime = 2 * m_timeToMaxVelocity;
      m_profileMaxVelocity = m_acceleration * m_timeToMaxVelocity;
    } else {
      m_timeFromMaxVelocity = m_timeToMaxVelocity + timeAtMaxV;
      m_totalTime = m_timeFromMaxVelocity + m_timeToMaxVelocity;
      m_profileMaxVelocity = m_velocity;
    }

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
   */
  public void setMaxVelocity(double velocity) {
    m_mutex.lock();
    m_velocity = velocity;
    m_mutex.unlock();
  }

  /**
   * Returns maximum velocity of profile.
   */
  public double getMaxVelocity() {
    m_mutex.lock();
    try {
      return m_velocity;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Sets time to maximum velocity of profile from rest.
   */
  public void setTimeToMaxV(double timeToMaxV) {
    m_mutex.lock();
    m_acceleration = m_velocity / timeToMaxV;
    m_mutex.unlock();
  }

  protected State updateReference(double currentTime) {
    m_mutex.lock();

    if (currentTime < m_timeToMaxVelocity) {
      // Accelerate up
      m_ref.m_acceleration = m_acceleration;
      m_ref.m_velocity = m_ref.m_acceleration * currentTime;
    } else if (currentTime < m_timeFromMaxVelocity) {
      // Maintain max velocity
      m_ref.m_acceleration = 0.0;
      m_ref.m_velocity = m_profileMaxVelocity;
    } else if (currentTime < m_totalTime) {
      // Accelerate down
      double decelTime = currentTime - m_timeFromMaxVelocity;
      m_ref.m_acceleration = -m_acceleration;
      m_ref.m_velocity = m_profileMaxVelocity + m_ref.m_acceleration * decelTime;
    }

    if (currentTime < m_totalTime) {
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
