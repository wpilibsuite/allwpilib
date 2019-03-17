package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.experimental.controller.ControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

/**
 * A subsystem uses a PIDController to control an output.  The controller is run asynchronously
 * by a separate thread with the period specified by the controller.
 */
public abstract class AsynchronousPIDSubsystem extends SendableSubsystemBase {

  private final PIDController m_controller;
  private final ControllerRunner m_runner;

  /**
   * Creates a new AsynchronousPIDSubsystem.
   *
   * @param controller the controller to use
   */
  public AsynchronousPIDSubsystem(PIDController controller) {
    m_controller = controller;
    m_runner = new ControllerRunner(m_controller, this::useOutput);
  }

  @Override
  public void periodic() {
    m_controller.setReference(getReference());
  }

  /**
   * Uses the output from the PIDController.  Should be synchronized to remain threadsafe.
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
   * Enable or disable the PIDController.
   *
   * @param enabled whether the controller is enabled
   */
  public void setEnabled(boolean enabled) {
    if (enabled) {
      m_runner.enable();
    } else {
      m_runner.disable();
    }
  }
}
