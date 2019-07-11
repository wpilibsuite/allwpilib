/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import java.util.concurrent.locks.ReentrantLock;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

public class PIDControllerRunner extends SendableBase {
  private final Notifier m_notifier = new Notifier(this::run);
  private final PIDController m_controller;
  private final DoubleConsumer m_controllerOutput;
  private final DoubleSupplier m_measurementSource;
  private boolean m_enabled;

  private final ReentrantLock m_thisMutex = new ReentrantLock();

  // Ensures when disable() is called, m_controllerOutput() won't run if
  // Controller.update() is already running at that time.
  private final ReentrantLock m_outputMutex = new ReentrantLock();

  /**
   * Allocates a PIDControllerRunner.
   *
   * @param controller        The controller on which to call update().
   * @param measurementSource The function that supplies the current process variable measurement.
   * @param controllerOutput  The function which updates the plant using the controller output
   *                          passed as the argument.
   */
  public PIDControllerRunner(PIDController controller, DoubleSupplier measurementSource,
                          DoubleConsumer controllerOutput) {
    m_controller = controller;
    m_controllerOutput = controllerOutput;
    m_measurementSource = measurementSource;
    m_notifier.startPeriodic(m_controller.getPeriod());
  }

  /**
   * Begins running the controller.
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
   * Stops running the controller.
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
          // Don't block other PIDControllerRunner operations on output
          m_thisMutex.unlock();

          m_controllerOutput.accept(m_controller.calculate(m_measurementSource.getAsDouble()));
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

  @Override
  public void initSendable(SendableBuilder builder) {
    m_controller.initSendable(builder);
    builder.setSafeState(this::disable);
    builder.addBooleanProperty("enabled", this::isEnabled, enabled -> {
      if (enabled) {
        enable();
      } else {
        disable();
      }
    });
  }
}
