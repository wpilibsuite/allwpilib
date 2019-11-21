/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

/**
 * A subsystem that generates and runs trapezoidal motion profiles automatically.  The user
 * specifies how to use the current state of the motion profile by overriding the `useState` method.
 */
public abstract class TrapezoidProfileSubsystem extends SubsystemBase {
  private final double m_period;
  private final TrapezoidProfile.Constraints m_constraints;

  private TrapezoidProfile.State m_state;

  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints     The constraints (maximum velocity and acceleration) for the profiles.
   * @param initialPosition The initial position of the controller mechanism when the subsystem
   *                        is constructed.
   */
  public TrapezoidProfileSubsystem(TrapezoidProfile.Constraints constraints,
                                   double initialPosition) {
    m_constraints = constraints;
    m_state = new TrapezoidProfile.State(initialPosition, 0);
    m_period = 0.02;
  }

  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints     The constraints (maximum velocity and acceleration) for the profiles.
   * @param initialPosition The initial position of the controller mechanism when the subsystem
   *                        is constructed.
   * @param period          The period of the main robot loop, in seconds.
   */
  public TrapezoidProfileSubsystem(TrapezoidProfile.Constraints constraints,
                                   double initialPosition,
                                   double period) {
    m_constraints = constraints;
    m_state = new TrapezoidProfile.State(initialPosition, 0);
    m_period = period;
  }

  @Override
  public void periodic() {
    var profile = new TrapezoidProfile(m_constraints, getGoal(), m_state);
    m_state = profile.calculate(m_period);
    useState(m_state);
  }

  /**
   * Users should override this to return the goal state for the subsystem's motion profile.
   *
   * @return The goal state for the subsystem's motion profile.
   */
  public abstract TrapezoidProfile.State getGoal();

  /**
   * Users should override this to consume the current state of the motion profile.
   *
   * @param state The current state of the motion profile.
   */
  protected abstract void useState(TrapezoidProfile.State state);
}
