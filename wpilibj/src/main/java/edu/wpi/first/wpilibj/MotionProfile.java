/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.locks.ReentrantLock;

/**
 * Base class for all types of motion profile controllers.
 */
public abstract class MotionProfile {
  protected class State {
    public double m_displacement;
    public double m_velocity;
    public double m_acceleration;

    public State(double displacement, double velocity, double acceleration) {
      m_displacement = displacement;
      m_velocity = velocity;
      m_acceleration = acceleration;
    }
  }

  // Use this to make UpdateSetpoint() and SetGoal() thread-safe
  protected ReentrantLock m_mutex = new ReentrantLock();

  private PIDOutput m_output;
  private double m_period;
  private Timer m_timer = new Timer();
  private Notifier m_notifier = new Notifier(this::update);

  protected double m_goal = 0.0;

  // Current reference (displacement, velocity, acceleration)
  protected State m_ref = new State(0.0, 0.0, 0.0);

  protected double m_lastTime = 0.0;
  protected double m_totalTime = Double.POSITIVE_INFINITY;

  /**
   * Constructs a motion profile that updates an output at the given period.
   *
   * @param output The output to update.
   * @param period The period after which to update the output. The default is
   *               50ms.
   */
  public MotionProfile(PIDOutput output, double period) {
    m_output = output;
    m_period = period;

    m_timer.start();
    m_notifier.startPeriodic(period);
  }

  /**
   * Constructs a motion profile.
   *
   * @param output The output to update.
   */
  public MotionProfile(PIDOutput output) {
    this(output, 0.05);
  }

  /**
   * Starts periodic output.
   */
  public void enable() {
    m_notifier.startPeriodic(m_period);
  }

  /**
   * Stops periodic output.
   */
  public void disable() {
    m_notifier.stop();
  }

  public abstract void setGoal(double goal, double curSource);

  /**
   * Returns profile's goal state.
   */
  public double getGoal() {
    m_mutex.lock();
    try {
      return m_goal;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Returns profile's current state.
   */
  public double getReference() {
    m_mutex.lock();
    try {
      return m_ref.m_displacement;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Returns true if motion profile has reached goal state.
   */
  public boolean atGoal() {
    m_mutex.lock();
    try {
      return m_timer.get() >= m_totalTime || Math.abs(m_goal - m_ref.m_displacement) < 0.001;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Returns the total profile time.
   */
  public double getTotalTime() {
    return m_totalTime;
  }

  /**
   * Resets profile time.
   */
  public void reset() {
    m_lastTime = 0.0;
    m_timer.reset();
  }

  protected abstract State updateReference(double currentTime);

  /**
   * Used by the Notifier to update the output with the latest reference.
   */
  private void update() {
    m_mutex.lock();
    try {
      m_ref = updateReference(m_timer.get());
      m_output.pidWrite(m_ref.m_displacement);
    } finally {
      m_mutex.unlock();
    }
  }
}
