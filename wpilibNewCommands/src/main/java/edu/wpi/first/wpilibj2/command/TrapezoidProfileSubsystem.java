// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.trajectory.TrapezoidProfile;

/**
 * A subsystem that generates and runs trapezoidal motion profiles automatically. The user specifies
 * how to use the current state of the motion profile by overriding the `useState` method.
 *
 * <p>This class is provided by the NewCommands VendorDep
 *
 * @deprecated Use a TrapezoidProfile instead
 */
@Deprecated(forRemoval = true, since = "2025")
public abstract class TrapezoidProfileSubsystem extends SubsystemBase {
  private final double m_period;
  private final TrapezoidProfile m_profile;

  private TrapezoidProfile.State m_state;
  private TrapezoidProfile.State m_goal;

  private boolean m_enabled = true;

  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints The constraints (maximum velocity and acceleration) for the profiles.
   * @param initialPosition The initial position of the controlled mechanism when the subsystem is
   *     constructed.
   * @param period The period of the main robot loop, in seconds.
   */
  public TrapezoidProfileSubsystem(
      TrapezoidProfile.Constraints constraints, double initialPosition, double period) {
    requireNonNullParam(constraints, "constraints", "TrapezoidProfileSubsystem");
    m_profile = new TrapezoidProfile(constraints);
    m_state = new TrapezoidProfile.State(initialPosition, 0);
    setGoal(initialPosition);
    m_period = period;
  }

  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints The constraints (maximum velocity and acceleration) for the profiles.
   * @param initialPosition The initial position of the controlled mechanism when the subsystem is
   *     constructed.
   */
  public TrapezoidProfileSubsystem(
      TrapezoidProfile.Constraints constraints, double initialPosition) {
    this(constraints, initialPosition, 0.02);
  }

  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints The constraints (maximum velocity and acceleration) for the profiles.
   */
  public TrapezoidProfileSubsystem(TrapezoidProfile.Constraints constraints) {
    this(constraints, 0, 0.02);
  }

  @Override
  public void periodic() {
    m_state = m_profile.calculate(m_period, m_state, m_goal);
    if (m_enabled) {
      useState(m_state);
    }
  }

  /**
   * Sets the goal state for the subsystem.
   *
   * @param goal The goal state for the subsystem's motion profile.
   */
  public final void setGoal(TrapezoidProfile.State goal) {
    m_goal = goal;
  }

  /**
   * Sets the goal state for the subsystem. Goal velocity assumed to be zero.
   *
   * @param goal The goal position for the subsystem's motion profile.
   */
  public final void setGoal(double goal) {
    setGoal(new TrapezoidProfile.State(goal, 0));
  }

  /** Enable the TrapezoidProfileSubsystem's output. */
  public void enable() {
    m_enabled = true;
  }

  /** Disable the TrapezoidProfileSubsystem's output. */
  public void disable() {
    m_enabled = false;
  }

  /**
   * Users should override this to consume the current state of the motion profile.
   *
   * @param state The current state of the motion profile.
   */
  protected abstract void useState(TrapezoidProfile.State state);
}
