package edu.wpi.first.wpilibj.experimental.controller;

import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

/**
 * A convenience wrapper for running a {@link Controller} synchronously, allowing the user to pass
 * in functions for reference, measurement, and output.  Said functions have no need to be
 * thread-safe.  {@link SynchronousControllerRunner#run()} must be called synchronously from the
 * main robot loop by the user.
 */
public class SynchronousControllerRunner {
  private final Controller m_controller;
  private final DoubleSupplier m_referenceSource;
  private final DoubleSupplier m_measurementSource;
  private final DoubleConsumer m_controllerOutput;

  /**
   * Creates a new SynchronousControllerRunner.
   *
   * @param controller        The controller to run.
   * @param referenceSource   The source for the reference value for the controller.
   * @param measurementSource The source for the process variable value for the controller.
   * @param controllerOutput  The method that uses the output of the controller.
   */
  public SynchronousControllerRunner(Controller controller,
                                     DoubleSupplier referenceSource,
                                     DoubleSupplier measurementSource,
                                     DoubleConsumer controllerOutput) {
    m_controller = controller;
    m_referenceSource = referenceSource;
    m_measurementSource = measurementSource;
    m_controllerOutput = controllerOutput;
  }

  /**
   * Runs the controller.  Must be called synchronously from the main robot loop.
   */
  public void run() {
    m_controllerOutput.accept(m_controller.calculate(
        m_referenceSource.getAsDouble(), m_measurementSource.getAsDouble()));
  }

  /**
   * Sets the output to zero.
   */
  public void stop() {
    m_controllerOutput.accept(0);
  }
}
