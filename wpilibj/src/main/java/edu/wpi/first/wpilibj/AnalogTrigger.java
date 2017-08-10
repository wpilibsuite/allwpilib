/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;
import edu.wpi.first.wpilibj.hal.AnalogJNI;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.util.BoundaryException;

/**
 * Class for creating and configuring Analog Triggers.
 */
public class AnalogTrigger {

  /**
   * Exceptions dealing with improper operation of the Analog trigger.
   */
  public class AnalogTriggerException extends RuntimeException {

    /**
     * Create a new exception with the given message.
     *
     * @param message the message to pass with the exception
     */
    public AnalogTriggerException(String message) {
      super(message);
    }

  }

  /**
   * Where the analog trigger is attached.
   */
  protected int m_port;
  protected int m_index;
  protected AnalogInput m_analogInput = null;
  protected boolean m_ownsAnalog = false;

  /**
   * Constructor for an analog trigger given a channel number.
   *
   * @param channel the port to use for the analog trigger
   */
  public AnalogTrigger(final int channel) {
    this(new AnalogInput(channel));
    m_ownsAnalog = true;
  }

  /**
   * Construct an analog trigger given an analog channel. This should be used in the case of sharing
   * an analog channel between the trigger and an analog input object.
   *
   * @param channel the AnalogInput to use for the analog trigger
   */
  public AnalogTrigger(AnalogInput channel) {
    m_analogInput = channel;
    ByteBuffer index = ByteBuffer.allocateDirect(4);
    index.order(ByteOrder.LITTLE_ENDIAN);

    m_port =
        AnalogJNI.initializeAnalogTrigger(channel.m_port, index.asIntBuffer());
    m_index = index.asIntBuffer().get(0);

    HAL.report(tResourceType.kResourceType_AnalogTrigger, channel.getChannel());
  }

  /**
   * Release the resources used by this object.
   */
  public void free() {
    AnalogJNI.cleanAnalogTrigger(m_port);
    m_port = 0;
    if (m_ownsAnalog && m_analogInput != null) {
      m_analogInput.free();
    }
  }

  /**
   * Set the upper and lower limits of the analog trigger. The limits are given in ADC codes. If
   * oversampling is used, the units must be scaled appropriately.
   *
   * @param lower the lower raw limit
   * @param upper the upper raw limit
   */
  public void setLimitsRaw(final int lower, final int upper) {
    if (lower > upper) {
      throw new BoundaryException("Lower bound is greater than upper");
    }
    AnalogJNI.setAnalogTriggerLimitsRaw(m_port, lower, upper);
  }

  /**
   * Set the upper and lower limits of the analog trigger. The limits are given as floating point
   * voltage values.
   *
   * @param lower the lower voltage limit
   * @param upper the upper voltage limit
   */
  public void setLimitsVoltage(double lower, double upper) {
    if (lower > upper) {
      throw new BoundaryException("Lower bound is greater than upper bound");
    }
    AnalogJNI.setAnalogTriggerLimitsVoltage(m_port, lower, upper);
  }

  /**
   * Configure the analog trigger to use the averaged vs. raw values. If the value is true, then the
   * averaged value is selected for the analog trigger, otherwise the immediate value is used.
   *
   * @param useAveragedValue true to use an averaged value, false otherwise
   */
  public void setAveraged(boolean useAveragedValue) {
    AnalogJNI.setAnalogTriggerAveraged(m_port, useAveragedValue);
  }

  /**
   * Configure the analog trigger to use a filtered value. The analog trigger will operate with a 3
   * point average rejection filter. This is designed to help with 360 degree pot applications for
   * the period where the pot crosses through zero.
   *
   * @param useFilteredValue true to use a filterd value, false otherwise
   */
  public void setFiltered(boolean useFilteredValue) {
    AnalogJNI.setAnalogTriggerFiltered(m_port, useFilteredValue);
  }

  /**
   * Return the index of the analog trigger. This is the FPGA index of this analog trigger
   * instance.
   *
   * @return The index of the analog trigger.
   */
  public int getIndex() {
    return m_index;
  }

  /**
   * Return the InWindow output of the analog trigger. True if the analog input is between the upper
   * and lower limits.
   *
   * @return The InWindow output of the analog trigger.
   */
  public boolean getInWindow() {
    return AnalogJNI.getAnalogTriggerInWindow(m_port);
  }

  /**
   * Return the TriggerState output of the analog trigger. True if above upper limit. False if below
   * lower limit. If in Hysteresis, maintain previous state.
   *
   * @return The TriggerState output of the analog trigger.
   */
  public boolean getTriggerState() {
    return AnalogJNI.getAnalogTriggerTriggerState(m_port);
  }

  /**
   * Creates an AnalogTriggerOutput object. Gets an output object that can be used for routing.
   * Caller is responsible for deleting the AnalogTriggerOutput object.
   *
   * @param type An enum of the type of output object to create.
   * @return A pointer to a new AnalogTriggerOutput object.
   */
  public AnalogTriggerOutput createOutput(AnalogTriggerType type) {
    return new AnalogTriggerOutput(this, type);
  }
}
