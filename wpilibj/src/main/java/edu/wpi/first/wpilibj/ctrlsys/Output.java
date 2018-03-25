/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;
import java.util.TimerTask;

import edu.wpi.first.wpilibj.Controller;
import edu.wpi.first.wpilibj.PIDOutput;

/**
 * INode adapter for PIDOutput subclasses.
 *
 * <p>Wraps a PIDOutput object in the INode interface by calling PIDWrite() on it at a regular
 * interval specified in the constructor. This is called in a separate thread.
 */
public class Output implements Controller {
  private final INode m_input;
  private final PIDOutput m_output;
  private double m_period;

  private final java.util.Timer m_timer = new java.util.Timer();
  private final ReentrantLock m_mutex = new ReentrantLock();

  private double m_minU = -1.0;
  private double m_maxU = 1.0;

  private class OutputTask extends TimerTask {
    private Output m_output;

    OutputTask(Output output) {
      m_output = output;
    }

    @Override
    public void run() {
      m_output.outputFunc();
    }
  }

  /**
   * Calls PIDWrite() on the output at a regular interval.
   *
   * @param input the node that is used to get values
   * @param output the PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations.
   */
  public Output(INode input, PIDOutput output, double period) {
    m_input = input;
    m_output = output;
    m_period = period;

    m_input.setCallback(this);
  }

  /**
   * Calls PIDWrite() on the output at a regular interval.
   *
   * @param input the node that is used to get values
   * @param output the PIDOutput object that is set to the output value
   */
  public Output(INode input, PIDOutput output) {
    this(input, output, INode.DEFAULT_PERIOD);
  }

  /**
   * Starts closed loop control.
   */
  @Override
  public void enable() {
    m_timer.schedule(new OutputTask(this), 0, (long) (m_period * 1000));
  }

  /**
   * Stops closed loop control.
   */
  @Override
  public void disable() {
    m_timer.cancel();

    m_output.pidWrite(0.0);
  }

  /**
   * Set minimum and maximum control action. U designates control action.
   *
   * @param minU minimum control action
   * @param maxU maximum control action
   */
  public void setRange(double minU, double maxU) {
    m_mutex.lock();
    m_minU = minU;
    m_maxU = maxU;
    m_mutex.unlock();
  }

  protected void outputFunc() {
    double controlAction = m_input.getOutput();

    m_mutex.lock();

    if (controlAction > m_maxU) {
      m_output.pidWrite(m_maxU);
    } else if (controlAction < m_minU) {
      m_output.pidWrite(m_minU);
    } else {
      m_output.pidWrite(controlAction);
    }

    m_mutex.unlock();
  }
}
