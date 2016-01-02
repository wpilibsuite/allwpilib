/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.AnalogJNI;

import java.nio.IntBuffer;

/**
 * Class to represent a specific output from an analog trigger. This class is
 * used to get the current output value and also as a DigitalSource to provide
 * routing of an output to digital subsystems on the FPGA such as Counter,
 * Encoder, and Interrupt.
 *
 * The TriggerState output indicates the primary output value of the trigger. If
 * the analog signal is less than the lower limit, the output is false. If the
 * analog value is greater than the upper limit, then the output is true. If the
 * analog value is in between, then the trigger output state maintains its most
 * recent value.
 *
 * The InWindow output indicates whether or not the analog signal is inside the
 * range defined by the limits.
 *
 * The RisingPulse and FallingPulse outputs detect an instantaneous transition
 * from above the upper limit to below the lower limit, and vise versa. These
 * pulses represent a rollover condition of a sensor and can be routed to an up
 * / down couter or to interrupts. Because the outputs generate a pulse, they
 * cannot be read directly. To help ensure that a rollover condition is not
 * missed, there is an average rejection filter available that operates on the
 * upper 8 bits of a 12 bit number and selects the nearest outlyer of 3 samples.
 * This will reject a sample that is (due to averaging or sampling) errantly
 * between the two limits. This filter will fail if more than one sample in a
 * row is errantly in between the two limits. You may see this problem if
 * attempting to use this feature with a mechanical rollover sensor, such as a
 * 360 degree no-stop potentiometer without signal conditioning, because the
 * rollover transition is not sharp / clean enough. Using the averaging engine
 * may help with this, but rotational speeds of the sensor will then be limited.
 */
public class AnalogTriggerOutput extends DigitalSource {

  /**
   * Exceptions dealing with improper operation of the Analog trigger output
   */
  public class AnalogTriggerOutputException extends RuntimeException {

    /**
     * Create a new exception with the given message
     *
     * @param message the message to pass with the exception
     */
    public AnalogTriggerOutputException(String message) {
      super(message);
    }

  }

  private final AnalogTrigger m_trigger;
  private final AnalogTriggerType m_outputType;

  /**
   * Create an object that represents one of the four outputs from an analog
   * trigger.
   *
   * Because this class derives from DigitalSource, it can be passed into
   * routing functions for Counter, Encoder, etc.
   *
   * @param trigger The trigger for which this is an output.
   * @param outputType An enum that specifies the output on the trigger to
   *        represent.
   */
  public AnalogTriggerOutput(AnalogTrigger trigger, final AnalogTriggerType outputType) {
    if (trigger == null)
      throw new NullPointerException("Analog Trigger given was null");
    if (outputType == null)
      throw new NullPointerException("Analog Trigger Type given was null");
    m_trigger = trigger;
    m_outputType = outputType;

    UsageReporting.report(tResourceType.kResourceType_AnalogTriggerOutput, trigger.getIndex(),
        outputType.value);
  }

  @Override
  public void free() {

  }

  /**
   * Get the state of the analog trigger output.
   *
   * @return The state of the analog trigger output.
   */
  public boolean get() {
    return AnalogJNI.getAnalogTriggerOutput(m_trigger.m_port, m_outputType.value);
  }

  @Override
  public int getChannelForRouting() {
    return (m_trigger.m_index << 2) + m_outputType.value;
  }

  @Override
  public byte getModuleForRouting() {
    return (byte) (m_trigger.m_index >> 2);
  }

  @Override
  public boolean getAnalogTriggerForRouting() {
    return true;
  }

  /**
   * Defines the state in which the AnalogTrigger triggers
   *$
   * @author jonathanleitschuh
   */
  public enum AnalogTriggerType {
    kInWindow(AnalogJNI.AnalogTriggerType.kInWindow), kState(AnalogJNI.AnalogTriggerType.kState), kRisingPulse(
        AnalogJNI.AnalogTriggerType.kRisingPulse), kFallingPulse(
        AnalogJNI.AnalogTriggerType.kFallingPulse);

    private final int value;

    private AnalogTriggerType(int value) {
      this.value = value;
    }
  }
}
