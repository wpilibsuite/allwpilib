/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.controller;

import java.util.concurrent.locks.ReentrantLock;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.Notifier;

/**
 * A runner that runs a controller asynchronously in a {@link Notifier}, allowing the user to pass
 * in functions for the measurement, reference, and output.  Said functions should be thread-safe.
 */
public class AsynchronousControllerRunner {
  private final Notifier m_notifier = new Notifier(this::run);
  private final Controller m_controller;
  private final DoubleSupplier m_referenceSource;
  private final DoubleSupplier m_measurementSource;
  private final DoubleConsumer m_controllerOutput;
  private boolean m_enabled;

  private final ReentrantLock m_thisMutex = new ReentrantLock();

  // Ensures when disable() is called, m_controllerOutput() won't run if
  // Controller.calculate() is already running at that time.
  private final ReentrantLock m_outputMutex = new ReentrantLock();

  /**
   * Creates a new ControllerRunner.
   *
   * @param controller        The controller to run.
   * @param referenceSource   A method providing the reference value for the controller.
   * @param measurementSource A method providing the process variable value for the controller.
   * @param controllerOutput  A method that uses the output of the controller.
   */
  public AsynchronousControllerRunner(Controller controller,
                                      DoubleSupplier referenceSource,
                                      DoubleSupplier measurementSource,
                                      DoubleConsumer controllerOutput) {
    m_controller = controller;
    m_referenceSource = referenceSource;
    m_measurementSource = measurementSource;
    m_controllerOutput = controllerOutput;
    m_notifier.startPeriodic(m_controller.getPeriod());
  }

  /**
   * Begin running the controller.
   */
  public void enable() {
    m_thisMutex.lock();
    try {
      m_enabled = true;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Stop running the controller.
   *
   * <p>This sets the output to zero before stopping.
   */
  public void disable() {
    // Ensures m_enabled modification and m_controllerOutput() call occur
    // atomically
    m_outputMutex.lock();
    try {
      m_thisMutex.lock();
      try {
        m_enabled = false;
      } finally {
        m_thisMutex.unlock();
      }

      m_controllerOutput.accept(0.0);
    } finally {
      m_outputMutex.unlock();
    }
  }

  /**
   * Returns whether controller is running.
   */
  public boolean isEnabled() {
    m_thisMutex.lock();
    try {
      return m_enabled;
    } finally {
      m_thisMutex.unlock();
    }
  }


  private void run() {
    // Ensures m_enabled check and m_controllerOutput() call occur atomically
    m_outputMutex.lock();
    try {
      m_thisMutex.lock();
      try {
        if (m_enabled) {
          // Don't block other ControllerRunner operations on output
          m_thisMutex.unlock();

          m_controllerOutput.accept(m_controller.calculate(
              m_referenceSource.getAsDouble(), m_measurementSource.getAsDouble()));
        }
      } finally {
        if (m_thisMutex.isHeldByCurrentThread()) {
          m_thisMutex.unlock();
        }
      }
    } finally {
      m_outputMutex.unlock();
    }
  }
}
