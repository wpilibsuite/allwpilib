/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.wpilibj.Timer;

/**
 * Base class for all types of motion profile controllers.
 */
public abstract class MotionProfile implements INode {
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

  protected Timer m_timer = new Timer();

  protected double m_goal = 0.0;

  // Current reference (displacement, velocity, acceleration)
  protected State m_ref = new State(0.0, 0.0, 0.0);

  protected double m_lastTime = 0.0;
  protected double m_timeTotal = Double.POSITIVE_INFINITY;

  public MotionProfile() {
    m_timer.start();
  }

  @Override
  public double getOutput() {
    m_mutex.lock();
    try {
      m_ref = updateSetpoint(m_timer.get());
      return m_ref.m_displacement;
    } finally {
      m_mutex.unlock();
    }
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
   * Returns true if motion profile has reached goal state.
   */
  public boolean atGoal() {
    m_mutex.lock();
    try {
      return m_lastTime >= m_timeTotal || Math.abs(m_goal - m_ref.m_displacement) < 0.001;
    } finally {
      m_mutex.unlock();
    }
  }

  public void reset() {
    m_lastTime = 0.0;
    m_timer.reset();
  }

  protected abstract State updateSetpoint(double currentTime);
}
