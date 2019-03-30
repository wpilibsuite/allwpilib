package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static java.util.Objects.requireNonNull;

/**
 * A subsystem that uses a {@link PIDController} to control an output.  The controller is run
 * synchronously from the subsystem's periodic() method.
 */
public abstract class SynchronousPIDSubsystem extends SendableSubsystemBase {

  protected final PIDController m_controller;
  protected boolean m_enabled;

  /**
   * Creates a new SynchronousPIDSubsystem.
   *
   * @param controller the PIDController to use
   */
  public SynchronousPIDSubsystem(PIDController controller) {
    requireNonNull(controller);
    m_controller = controller;
  }

  @Override
  public void periodic() {
    m_controller.setReference(getReference());

    if (m_enabled) {
      useOutput(m_controller.calculate(getReference(), getMeasurement()));
    }
  }

  public PIDController getController() {
    return m_controller;
  }

  /**
   * Uses the output from the PIDController.
   *
   * @param output the output of the PIDController
   */
  public abstract void useOutput(double output);

  /**
   * Returns the reference(setpoint) used by the PIDController.
   *
   * @return the reference (setpoint) to be used by the controller
   */
  public abstract double getReference();

  /**
   * Returns the measurement of the process variable used by the PIDController.
   *
   * @return the measurement of the process variable
   */
  public abstract double getMeasurement();

  /**
   * Enable or disable the PIDController.
   *
   * @param enabled whether the controller is enabled
   */
  public void setEnabled(boolean enabled) {
    m_enabled = enabled;
  }
}
